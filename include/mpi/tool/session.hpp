#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
class session
{
public:
  session           ()
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_session_create, (&native_))
  }
  explicit session  (MPI_T_pvar_session native)
  : native_(native)
  {
    
  }
  session           (const session&  that) = delete;
  session           (      session&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_PVAR_SESSION_NULL;
  }
  virtual ~session  ()
  {
    if (managed_ && native_)
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_session_free,  (&native_))
  }
  session& operator=(const session&  that) = delete;
  session& operator=(      session&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_)
        MPI_CHECK_ERROR_CODE(MPI_T_pvar_session_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_T_PVAR_SESSION_NULL;
    }
    return *this;
  }

  void               start  () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_start, (native_, MPI_T_PVAR_ALL_HANDLES))
  }
  void               stop   () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_stop , (native_, MPI_T_PVAR_ALL_HANDLES))
  }
  void               reset  () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_reset, (native_, MPI_T_PVAR_ALL_HANDLES))
  }

  [[nodiscard]]
  bool               managed() const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_T_pvar_session native () const
  {
    return native_;
  }

protected:
  bool               managed_ = false;
  MPI_T_pvar_session native_  = MPI_T_PVAR_SESSION_NULL;
};
}