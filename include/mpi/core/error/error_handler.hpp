#pragma once

#include <cstdint>
#include <functional>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class error_handler
{
public:
  explicit error_handler  (const std::function<void(MPI_Comm*, int*)>& function)
  {
    // TODO: FIX. TARGET() DOES NOT WORK!
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_errhandler, (*function.target<void(*)(MPI_Comm*, int*, ...)>(), &native_))
  }
  explicit error_handler  (const MPI_Errhandler native)
  : native_(native)
  {
    
  }
  error_handler           (const error_handler&  that) = delete;
  error_handler           (      error_handler&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_ERRHANDLER_NULL;
  }
  virtual ~error_handler  ()
  {
    if (managed_ && native_ != MPI_ERRHANDLER_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Errhandler_free, (&native_))
  }
  error_handler& operator=(const error_handler&  that) = delete;
  error_handler& operator=(      error_handler&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_ERRHANDLER_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Errhandler_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_ERRHANDLER_NULL;
    }
    return *this;
  }

protected:
  bool           managed_ = false;
  MPI_Errhandler native_  = MPI_ERRHANDLER_NULL;
};
}