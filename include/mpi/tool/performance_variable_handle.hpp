#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/performance_variable.hpp>
#include <mpi/tool/session.hpp>

namespace mpi::tool
{
template <typename type>
class performance_variable_handle
{
public:
  explicit performance_variable_handle  (const session& session, const std::int32_t index, type& data)
  : managed_(true), session_(session)
  {
    std::int32_t count;
    MPI_T_pvar_handle_alloc(session_.native(), index, static_cast<void*>(&data), &native_, &count);
  }
  explicit performance_variable_handle  (const session& session, const MPI_T_pvar_handle& native)
  : native_(native), session_(session)
  {
    
  }
  performance_variable_handle           (const performance_variable_handle&  that) = delete;
  performance_variable_handle           (      performance_variable_handle&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_), session_(temp.session_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_PVAR_HANDLE_NULL;
  }
  virtual ~performance_variable_handle  ()
  {
    if (managed_ && native_ != MPI_T_PVAR_HANDLE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_free, (session_.native(), &native_))
  }
  performance_variable_handle& operator=(const performance_variable_handle&  that) = delete;
  performance_variable_handle& operator=(      performance_variable_handle&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_T_PVAR_HANDLE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_free, (session_.native(), &native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_T_PVAR_HANDLE_NULL;
    }
    return *this;
  }

  void              start     () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_start, (session_.native(), native_))
  }
  void              stop      () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_stop , (session_.native(), native_))
  }
  void              reset     () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_reset, (session_.native(), native_))
  }

  type              read      () const
  {
    type result;
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_read     , (session_.native(), native_, static_cast<      void*>(&result)))
    return result;
  }
  type              read_reset() const
  {
    type result;
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_readreset, (session_.native(), native_, static_cast<      void*>(&result)))
    return result;
  }
  void              write     (const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_write    , (session_.native(), native_, static_cast<const void*>(&value)))
  }

  [[nodiscard]]
  bool              managed   () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_T_pvar_handle native    () const
  {
    return native_;
  }

protected:
  bool              managed_ = false;
  MPI_T_pvar_handle native_  = MPI_T_PVAR_HANDLE_NULL;
  const session&    session_ ;
};
}