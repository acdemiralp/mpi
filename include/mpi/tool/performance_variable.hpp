#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/performance_variable_information.hpp>

namespace mpi::tool
{
class session
{
public:
  explicit session  ()
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_session_create, (&native_))
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

class performance_variable
{
public:
  explicit performance_variable  ()
  {

  }
  performance_variable           (const performance_variable&  that) = default;
  performance_variable           (      performance_variable&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_PVAR_HANDLE_NULL;
  }
  virtual ~performance_variable  ()
  {

  }
  performance_variable& operator=(const performance_variable&  that) = default;
  performance_variable& operator=(      performance_variable&& temp) noexcept
  {
    if (this != &temp)
    {

    }
    return *this;
  }

  [[nodiscard]]
  bool              managed() const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_T_pvar_handle native () const
  {
    return native_;
  }

protected:
  bool              managed_ = false;
  MPI_T_pvar_handle native_  = MPI_T_PVAR_HANDLE_NULL;
};

inline std::int32_t                                  performance_variable_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_num, (&result))
  return result;
}
inline std::vector<performance_variable_information> performance_variable_info ()
{
  std::vector<performance_variable_information> result(performance_variable_count());

  return result;
}
}