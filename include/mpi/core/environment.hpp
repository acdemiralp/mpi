#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>
#include <mpi/core/thread_support.hpp>

namespace mpi
{
class environment
{
public:
  environment           (std::int32_t* argc, char*** argv)
  : should_finalize_(MPI_Init(argc, argv) == MPI_SUCCESS)
  {

  }
  environment           (std::int32_t* argc, char*** argv, thread_support required_thread_support)
  : should_finalize_(MPI_Init_thread(argc, argv, static_cast<std::int32_t>(required_thread_support), reinterpret_cast<std::int32_t*>(&provided_thread_support_)) == MPI_SUCCESS)
  {

  }
  environment           (const environment&  that) = delete;
  environment           (      environment&& temp) = delete;
  virtual ~environment  ()
  {
    if (should_finalize_)
      MPI_Finalize();
  }
  environment& operator=(const environment&  that) = delete;
  environment& operator=(      environment&& temp) = delete;

  static bool           initialized            ()
  {
    std::int32_t result;
    MPI_Initialized(&result);
    return static_cast<bool>(result);
  }
  static bool           finalized              ()
  {
    std::int32_t result;
    MPI_Finalized(&result);
    return static_cast<bool>(result);
  }

  static thread_support query_thread_support   ()
  {
    std::int32_t result;
    MPI_Query_thread(&result);
    return static_cast<thread_support>(result);
  }

  bool                  should_finalize        () const
  {
    return should_finalize_;
  }
  thread_support        provided_thread_support() const
  {
    return provided_thread_support_;
  }

protected:
  bool           should_finalize_         ;
  thread_support provided_thread_support_ = thread_support::single;
};
}