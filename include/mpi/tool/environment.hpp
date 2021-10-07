#pragma once

#include <cstdint>

#include <mpi/core/enums/thread_support.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
class environment
{
public:
  explicit environment  (const thread_support required_thread_support)
  {
    MPI_CHECK_ERROR_CODE(MPI_T_init_thread, (static_cast<std::int32_t>(required_thread_support), reinterpret_cast<std::int32_t*>(&provided_thread_support_)))
  }
  environment           (const environment&  that) = delete;
  environment           (      environment&& temp) = delete;
  virtual ~environment  ()
  {
    MPI_CHECK_ERROR_CODE(MPI_T_finalize, ())
  }
  environment& operator=(const environment&  that) = delete;
  environment& operator=(      environment&& temp) = delete;

  [[nodiscard]]
  thread_support provided_thread_support() const
  {
    return provided_thread_support_;
  }

protected:
  thread_support provided_thread_support_;
};
}