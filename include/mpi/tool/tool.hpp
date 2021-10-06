#pragma once

#include <cstdint>

#include <mpi/core/enums/thread_support.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
class tool
{
public:
  explicit tool  (const thread_support required_thread_support)
  {
    std::int32_t provided_thread_support; // Unused.
    MPI_CHECK_ERROR_CODE(MPI_T_init_thread, (static_cast<std::int32_t>(required_thread_support), &provided_thread_support))
  }
  tool           (const tool&  that) = delete;
  tool           (      tool&& temp) = delete;
  virtual ~tool  ()
  {
    MPI_T_finalize();
  }
  tool& operator=(const tool&  that) = delete;
  tool& operator=(      tool&& temp) = delete;
};
}