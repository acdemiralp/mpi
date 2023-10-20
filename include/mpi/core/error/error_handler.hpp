#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
namespace io
{
class file;
}

class error_handler
{
public:
  explicit error_handler  (const MPI_Errhandler native, const bool managed = false)
  : managed_(managed), native_(native)
  {
    
  }
  error_handler           (const error_handler&  that) = delete;
  error_handler           (      error_handler&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_ERRHANDLER_NULL;
  }
  virtual ~error_handler  () noexcept(false)
  {
    if (managed_ && native_ != MPI_ERRHANDLER_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Errhandler_free, (&native_))
  }
  error_handler& operator=(const error_handler&  that) = delete;
  error_handler& operator=(      error_handler&& temp) noexcept(false)
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

  [[nodiscard]]
  bool           managed() const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Errhandler native () const
  {
    return native_;
  }

protected:
  bool           managed_ = false;
  MPI_Errhandler native_  = MPI_ERRHANDLER_NULL;
};

class communicator_error_handler : public error_handler
{
public:
  using function_type = void (*) (MPI_Comm*, std::int32_t*, ...);

  explicit communicator_error_handler  (const function_type& function)
  : error_handler(MPI_ERRHANDLER_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_errhandler, (function, &native_))
  }
  explicit communicator_error_handler  (const MPI_Errhandler native, const bool managed = false)
  : error_handler(native, managed)
  {
    
  }
  communicator_error_handler           (const communicator_error_handler&  that) = delete ;
  communicator_error_handler           (      communicator_error_handler&& temp) = default;
 ~communicator_error_handler           () override                               = default;
  communicator_error_handler& operator=(const communicator_error_handler&  that) = delete ;
  communicator_error_handler& operator=(      communicator_error_handler&& temp) = default;

  friend class communicator;
};
  
class file_error_handler : public error_handler
{
public:
  using function_type = void (*) (MPI_File*, std::int32_t*, ...);

  explicit file_error_handler  (const function_type& function)
  : error_handler(MPI_ERRHANDLER_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_File_create_errhandler, (function, &native_))
  }
  explicit file_error_handler  (const MPI_Errhandler native, const bool managed = false)
  : error_handler(native, managed)
  {
    
  }
  file_error_handler           (const file_error_handler&  that) = delete ;
  file_error_handler           (      file_error_handler&& temp) = default;
 ~file_error_handler           () override                       = default;
  file_error_handler& operator=(const file_error_handler&  that) = delete ;
  file_error_handler& operator=(      file_error_handler&& temp) = default;

  friend class io::file;
};
  
class window_error_handler : public error_handler
{
public:
  using function_type = void (*) (MPI_Win*, std::int32_t*, ...);

  explicit window_error_handler  (const function_type& function)
  : error_handler(MPI_ERRHANDLER_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create_errhandler, (function, &native_))
  }
  explicit window_error_handler  (const MPI_Errhandler native, const bool managed = false)
  : error_handler(native, managed)
  {
    
  }
  window_error_handler           (const window_error_handler&  that) = delete ;
  window_error_handler           (      window_error_handler&& temp) = default;
 ~window_error_handler           () override                         = default;
  window_error_handler& operator=(const window_error_handler&  that) = delete ;
  window_error_handler& operator=(      window_error_handler&& temp) = default;

  friend class window;
};

#ifdef MPI_USE_LATEST
class session_error_handler : public error_handler
{
public:
  using function_type = void (*) (MPI_Session*, std::int32_t*, ...);

  explicit session_error_handler  (const function_type& function)
  : error_handler(MPI_ERRHANDLER_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Session_create_errhandler, (function, &native_))
  }
  explicit session_error_handler  (const MPI_Errhandler native, const bool managed = false)
  : error_handler(native, managed)
  {
    
  }
  session_error_handler           (const session_error_handler&  that) = delete ;
  session_error_handler           (      session_error_handler&& temp) = default;
 ~session_error_handler           () override                          = default;
  session_error_handler& operator=(const session_error_handler&  that) = delete ;
  session_error_handler& operator=(      session_error_handler&& temp) = default;

  friend class session;
};
#endif

inline const error_handler error_handler_fatal (MPI_ERRORS_ARE_FATAL);
inline const error_handler error_handler_return(MPI_ERRORS_RETURN   );
#ifdef MPI_ERRORS_ABORT
inline const error_handler error_handler_abort (MPI_ERRORS_ABORT    );
#endif
}