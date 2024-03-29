#pragma once

#include <functional>
#include <future>
#include <optional>
#include <utility>

#include <mpi/core/request.hpp>

namespace mpi
{
// The std::future       <type> corresponds to MPI requests        , as both are handles to operations which will complete in the future.
// The std::async        <type> corresponds to MPI immediate  calls, as both are started implicitly.
// The std::packaged_task<type> corresponds to MPI persistent calls, as both are started explicitly, possibly more than once.
class future
{
public:
  future           (request&& request) noexcept
  : request_(request.persistent() ? mpi::request(request.native(), false, true) : std::move(request)) // If persistent create and start an unmanaged copy, else move.
  {
    if (request_.persistent())
      request_.start();
  }
  future           (const future&  that) = delete ;
  future           (      future&& temp) = default;
  virtual ~future  ()                    = default;
  future& operator=(const future&  that) = delete ;
  future& operator=(      future&& temp) = default;

  [[nodiscard]]
  bool   valid   () const noexcept
  {
    // Note: Persistent requests are always valid until destruction.
    return request_.native() != MPI_REQUEST_NULL; 
  }
  [[nodiscard]]
  bool   is_ready() const noexcept
  {
    return state_ || request_.get_status() != std::nullopt;
  }

  void   wait    () // Forced non-const.
  {
    state_ = request_.wait();
  }
  status get     ()
  {
    wait();
    return *state_;
  }

  [[nodiscard]]
  future then    (const std::function<future(future)>& function)
  {
    return function(std::move(*this));
  }

protected:
  request               request_;
  std::optional<status> state_  ;
};

[[nodiscard]]
inline future make_ready_future()
{
  return request(MPI_REQUEST_NULL);
}
[[nodiscard]]
inline future when_all         (std::vector<request>& requests)
{
  return make_ready_future().then([&] (future future)
  {
    [[maybe_unused]] auto result = future.get();
    wait_all(requests);
    return make_ready_future();
  });
}
[[nodiscard]]
inline future when_any         (std::vector<request>& requests)
{
  return make_ready_future().then([&] (future future)
  {
    [[maybe_unused]] auto result = future.get();
    wait_any(requests);
    return make_ready_future();
  });
}
}
