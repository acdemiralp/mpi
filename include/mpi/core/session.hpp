#pragma once

#include <cstdint>

#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
//#ifdef MPI_USE_LATEST
class session
{
public:
  session           (const information& information, const session_error_handler& error_handler)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Session_init, (information.native(), error_handler.native(), &native_))
  }
  explicit session  (const MPI_Session  native)
  : native_(native)
  {
    
  }
  session           (const session&     that  ) = delete;
  session           (      session&&    temp  ) noexcept
  : managed_(temp.managed_), native_ (temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_SESSION_NULL;
  }
  virtual ~session  ()
  {
    if (managed_ && native_ != MPI_SESSION_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Session_finalize, (&native_))
  }
  session& operator=(const session&     that  ) = delete;
  session& operator=(      session&&    temp  ) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_SESSION_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Session_finalize, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_SESSION_NULL;
    }
    return *this;
  }
  
  [[nodiscard]]
  information        information       () const
  {
    mpi::information result;
    MPI_CHECK_ERROR_CODE(MPI_Session_get_info, (native_, &result.native_))
    return result;
  }

  [[nodiscard]]
  window_error_handler error_handler     () const
  {
    MPI_Errhandler result;
    MPI_CHECK_ERROR_CODE(MPI_Session_get_errhandler , (native_, &result))
    return window_error_handler(result);
  }
  void                 set_error_handler (const window_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Session_set_errhandler , (native_, value.native()))
  }
  void                 call_error_handler(const error_code& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Session_call_errhandler, (native_, value.native()))
  }

protected:
  bool        managed_ = false;
  MPI_Session native_  = MPI_SESSION_NULL;
};
//#endif
}