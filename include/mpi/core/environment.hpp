#pragma once

#include <cstdint>

#include <mpi/core/enums/thread_support.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class environment
{
public:
  explicit environment  (std::int32_t* argc = nullptr, char*** argv = nullptr)
  {
    MPI_CHECK_ERROR_CODE(MPI_Init, (argc, argv))
  }
  environment           (std::int32_t* argc          , char*** argv          , thread_support required_thread_support)
  {
    std::int32_t provided_thread_support; // Unused. Call this.thread_support() explicitly.
    MPI_CHECK_ERROR_CODE(MPI_Init_thread, (argc, argv, static_cast<std::int32_t>(required_thread_support), &provided_thread_support))
  }
  environment           (const environment&  that)          = delete;
  environment           (      environment&& temp) noexcept = delete;
  virtual ~environment  ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Finalize, ())
  }
  environment& operator=(const environment&  that)          = delete;
  environment& operator=(      environment&& temp) noexcept = delete;
};

inline bool           initialized         ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Initialized   , (&result))
  return static_cast<bool>(result);
}
inline bool           finalized           ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Finalized     , (&result))
  return static_cast<bool>(result);
}

inline thread_support query_thread_support()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Query_thread  , (&result))
  return static_cast<thread_support>(result);
}
inline bool           is_thread_main      ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Is_thread_main, (&result))
  return static_cast<bool>(result);
}
}