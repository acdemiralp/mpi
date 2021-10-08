#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class communicator_error_handler
{
public:
  using function_type = void (*) (MPI_Comm*, std::int32_t*, ...);

  explicit communicator_error_handler  (const function_type& function)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_errhandler, (function, &native_))
  }
  explicit communicator_error_handler  (const MPI_Errhandler native)
  : native_(native)
  {
    
  }
  communicator_error_handler           (const communicator_error_handler&  that) = delete;
  communicator_error_handler           (      communicator_error_handler&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_ERRHANDLER_NULL;
  }
  virtual ~communicator_error_handler  ()
  {
    if (managed_ && native_ != MPI_ERRHANDLER_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Errhandler_free, (&native_))
  }
  communicator_error_handler& operator=(const communicator_error_handler&  that) = delete;
  communicator_error_handler& operator=(      communicator_error_handler&& temp) noexcept
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

class file_error_handler
{
public:
  using function_type = void (*) (MPI_File*, std::int32_t*, ...);

  explicit file_error_handler  (const function_type& function)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_File_create_errhandler, (function, &native_))
  }
  explicit file_error_handler  (const MPI_Errhandler native)
  : native_(native)
  {
    
  }
  file_error_handler           (const file_error_handler&  that) = delete;
  file_error_handler           (      file_error_handler&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_ERRHANDLER_NULL;
  }
  virtual ~file_error_handler  ()
  {
    if (managed_ && native_ != MPI_ERRHANDLER_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Errhandler_free, (&native_))
  }
  file_error_handler& operator=(const file_error_handler&  that) = delete;
  file_error_handler& operator=(      file_error_handler&& temp) noexcept
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

class window_error_handler
{
public:
  using function_type = void (*) (MPI_Win*, std::int32_t*, ...);

  explicit window_error_handler  (const function_type& function)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create_errhandler, (function, &native_))
  }
  explicit window_error_handler  (const MPI_Errhandler native)
  : native_(native)
  {
    
  }
  window_error_handler           (const window_error_handler&  that) = delete;
  window_error_handler           (      window_error_handler&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_ERRHANDLER_NULL;
  }
  virtual ~window_error_handler  ()
  {
    if (managed_ && native_ != MPI_ERRHANDLER_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Errhandler_free, (&native_))
  }
  window_error_handler& operator=(const window_error_handler&  that) = delete;
  window_error_handler& operator=(      window_error_handler&& temp) noexcept
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