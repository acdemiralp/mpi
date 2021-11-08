#pragma once

#include <chrono>
#include <future>
#include <optional>

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
    // TODO CORRECT 
    //if (request_.persistent())
    //  request_.start();
  }
  future           (const future&  that) = delete ;
  future           (      future&& temp) = default;
  virtual ~future  ()                    = default;
  future& operator=(const future&  that) = delete ;
  future& operator=(      future&& temp) = default;

  [[nodiscard]]
  status             get       ()
  {
    wait();
    return *state_;
  }
  [[nodiscard]]
  bool               valid     () const noexcept
  {
    // Note: Persistent requests are always valid until destruction.
    return request_.native() != MPI_REQUEST_NULL; 
  }
  [[nodiscard]]
  bool               is_ready  () const noexcept
  {
    return state_ || request_.get_status() != std::nullopt;
  }

  void               wait      () // Forced non-const.
  {
    state_ = request_.wait();
  }
  template <typename representation, typename period>
  std::future_status wait_for  (const std::chrono::duration<representation, period>& duration) const
  {
    // TODO CORRECT 
    if (is_ready())
      return std::future_status::ready;

    std::this_thread::sleep_for(duration);

    if (is_ready())
      return std::future_status::ready;

    return std::future_status::timeout;
  }
  template <typename clock, typename duration>
  std::future_status wait_until(const std::chrono::time_point<clock, duration>&      time    ) const
  {
    // TODO CORRECT 
    if (is_ready())
      return std::future_status::ready;

    std::this_thread::sleep_until(time);

    if (is_ready())
      return std::future_status::ready;

    return std::future_status::timeout;
  }
  [[nodiscard]]
  future             then      (std::function<future(future)> function)
  {
    // TODO
    return std::move(*this);
  }

protected:
  request               request_;
  std::optional<status> state_  ;
};
// TODO: mpi::when_all, mpi::when_any, mpi::make_ready_future.
// TODO: mpi::shared_future. mpi::shared_future mpi::future::share() noexcept.
}