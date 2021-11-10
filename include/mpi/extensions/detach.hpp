#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <mpi/core/request.hpp>
#include <mpi/core/status.hpp>

// This is a C++20 implementation of https://github.com/RWTH-HPC/mpi-detach with several modifications:
// - Use RAII for initialization and finalization.
// - Check persistence of a request in mpi::request instead.
// - Stati are never ignored.
// - Use mpi::request  instead of MPI_Request.
// - Use mpi::status   instead of MPI_Status .
// - Use std::function instead of function pointers.
// - Use captures      instead of void* argument.
namespace mpi
{
#ifdef MPI_USE_DETACH
class detach_context
{
public:
  using detach_function     = std::function<void(const status&)>;
  using detach_all_function = std::function<void(const std::vector<status>&)>;

  struct detach_state
  {
    detach_state(request& req, detach_function func)
    : request (req.persistent() ? mpi::request(req.native(), false, true) : std::move(req)) // If persistent create an unmanaged copy, else move.
    , function(std::move(func))
    {

    }
  
    request         request ;
    detach_function function;
    status          status  ;
  };
  struct detach_all_state
  {
    detach_all_state(std::vector<request>& reqs, detach_all_function func)
    : function(std::move(func))
    , stati   (reqs.size())
    {
      requests.reserve(reqs.size());
      for (auto& req : reqs)
        requests.push_back(req.persistent() ? request(req.native(), false, true) : std::move(req)); // If persistent create an unmanaged copy, else move.
    }
  
    std::vector<request> requests;
    detach_all_function  function;
    std::vector<status>  stati   ;
  };

  explicit detach_context  (const bool create_progress_thread = true)
  : running_(true)
  {
    if (create_progress_thread)
      progress_thread_ = std::thread([&]
      {
        while (running_ || !single_requests_.empty() || !all_requests_.empty())
        {
          do
          {
            std::unique_lock lock(list_mutex_);

            if (!single_requests_queue_.empty())
              single_requests_.splice(single_requests_.begin(), single_requests_queue_);

            if (!all_requests_queue_.empty())
              all_requests_   .splice(all_requests_   .begin(), all_requests_queue_   );

            if (single_requests_.empty() && all_requests_.empty())
              while (running_ && single_requests_queue_.empty() && all_requests_queue_.empty())
              {
                list_condition_variable_.wait(lock);
              }
          } while (running_ && single_requests_.empty() && all_requests_.empty());

          progress();

          std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
      });
  }
  detach_context           (const detach_context&  that) = delete;
  detach_context           (      detach_context&& temp) = delete;
  virtual ~detach_context  ()
  {
    if (progress_thread_)
    {
      {
        std::unique_lock lock(list_mutex_);
        while (!single_requests_queue_.empty() || !all_requests_queue_.empty()) 
        {
          list_condition_variable_.notify_one();
          lock.unlock();
          std::this_thread::sleep_for(std::chrono::milliseconds(2));
          lock.lock  ();
        }
        running_ = false;
        list_condition_variable_.notify_one();
      }
      progress_thread_->join();
    }
    else
    {
      while (!single_requests_queue_.empty() || 
             !single_requests_      .empty() || 
             !all_requests_queue_   .empty() ||
             !all_requests_         .empty())
        progress();
    }
  }
  detach_context& operator=(const detach_context&  that) = delete;
  detach_context& operator=(      detach_context&& temp) = delete;

  void progress            ()
  {
    if (!progress_mutex_.try_lock())
      return;

    {
      std::unique_lock lock(list_mutex_);

      if (!single_requests_queue_.empty())
        single_requests_.splice(single_requests_.begin(), single_requests_queue_);

      if (!all_requests_queue_.empty())
        all_requests_   .splice(all_requests_   .begin(), all_requests_queue_   );
    }

    if (!single_requests_.empty()) 
    {
      auto   iterator  = single_requests_.begin();
      while (iterator != single_requests_.end  ())
      {
        if (auto status = (*iterator)->request.test())
        {
          (*iterator)->function((*iterator)->status);
          delete *iterator;
          iterator = single_requests_.erase(iterator);
        }
        else 
          ++iterator;
      }
    }
    if (!all_requests_   .empty()) 
    {
      auto   iterator  = all_requests_.begin();
      while (iterator != all_requests_.end  ()) 
      {
        if (auto status = test_all((*iterator)->requests))
        {
          (*iterator)->function((*iterator)->stati);
          delete *iterator;
          iterator = all_requests_.erase(iterator);
        }
        else
          ++iterator;
      }
    }

    progress_mutex_.unlock();
  }
  void detach              (request&              request , const detach_function&     callback)
  {
    if (const auto status = request.test()) 
      callback(*status);
    else 
    {
      std::unique_lock lock(list_mutex_);
      single_requests_queue_    .push_back (new detach_state(request, callback));
      list_condition_variable_  .notify_one();
    }
  }
  void detach_each         (std::vector<request>& requests, const detach_function&     callback)
  {
    for (auto& request : requests)
    {
      if (auto status = request.test())
        callback(*status);
      else 
      {
        std::unique_lock lock(list_mutex_);
        single_requests_queue_  .push_back (new detach_state(request, callback));
        list_condition_variable_.notify_one();
      }
    }
  }
  void detach_all          (std::vector<request>& requests, const detach_all_function& callback)
  {
    if (const auto stati = test_all(requests))
      callback(*stati);
    else 
    {
      std::unique_lock lock(list_mutex_);
      all_requests_queue_       .push_back (new detach_all_state(requests, callback));
      list_condition_variable_  .notify_one();
    }
  }

protected:
  std::atomic<bool>            running_                ;
  std::optional<std::thread>   progress_thread_        ;
  std::mutex                   progress_mutex_         ;
  std::mutex                   list_mutex_             ;
  std::condition_variable      list_condition_variable_;
  std::list<detach_state*>     single_requests_queue_  ;
  std::list<detach_state*>     single_requests_        ;
  std::list<detach_all_state*> all_requests_queue_     ;
  std::list<detach_all_state*> all_requests_           ;
};

// Arguments are only meaningful on the first call to this function.
detach_context& get_detach_context(const bool create_progress_thread = true) 
{
  static detach_context context(create_progress_thread);
  return context;
}

inline void request::detach(const detach_context::detach_function& function)
{
  get_detach_context().detach(*this, function);
}

inline void detach_each(std::vector<request>& requests, const detach_context::detach_function&     function)
{
  get_detach_context().detach_each(requests, function);
}
inline void detach_all (std::vector<request>& requests, const detach_context::detach_all_function& function)
{
  get_detach_context().detach_all (requests, function);
}
#endif
}