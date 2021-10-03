#pragma once

#include <cstdint>
#include <functional>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/enums/return_code.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class error_handler
{
public:
  error_handler           (std::function<void(communicator&, return_code)> function)
  : function_(function)
  {
    MPI_Comm_create_errhandler([ ] (MPI_Comm* raw_communicator, std::int32_t* raw_return_code, ...)
    {
      function_(communicator(*raw_communicator), return_code(*raw_communicator));
    }, &native_);
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
      MPI_Errhandler_free(&native_);
  }
  error_handler& operator=(const error_handler&  that) = delete;
  error_handler& operator=(      error_handler&& temp) noexcept
  {
    if (this != &temp)
    {
      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_ERRHANDLER_NULL;
    }
    return *this;
  }

protected:
  bool                                            managed_ = false;
  MPI_Errhandler                                  native_  = MPI_ERRHANDLER_NULL;
  std::function<void(communicator&, return_code)> function_;
};
}