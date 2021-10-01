#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>
#include <mpi/core/thread_support.hpp>

namespace mpi
{
class environment
{
public:
  explicit environment  (std::int32_t* argc = nullptr, char*** argv = nullptr)
  {
    managed_ = MPI_Init(argc, argv) == MPI_SUCCESS;
  }
  environment           (std::int32_t* argc, char*** argv, thread_support required_thread_support)
  {
    std::int32_t provided_thread_support; // Unused. Call query_thread_support explicitly.
    managed_ = MPI_Init_thread(argc, argv, static_cast<std::int32_t>(required_thread_support), &provided_thread_support) == MPI_SUCCESS;
  }
  environment           (const environment&  that) = delete;
  environment           (      environment&& temp) noexcept
  : managed_(temp.managed_)
  {
    temp.managed_ = false;
  }
  virtual ~environment  ()
  {
    if (managed_)
      MPI_Finalize();
  }
  environment& operator=(const environment&  that) = delete;
  environment& operator=(      environment&& temp) noexcept
  {
    if (this != &temp)
    {
      managed_      = temp.managed_;
      temp.managed_ = false;
    }
    return *this;
  }

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

  [[nodiscard]]
  bool                  managed                () const
  {
    return managed_;
  }

protected:
  bool managed_ = false;
};
}