#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/control_variable_information.hpp>

namespace mpi::tool
{
class control_variable
{
public:
  template <typename type>
  explicit control_variable  (const std::int32_t index, type& data)
  : managed_(true)
  {
    std::int32_t count;
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_alloc, (index, static_cast<void*>(&data), &native_, &count))
  }
  explicit control_variable  (const MPI_T_cvar_handle& native)
  : native_(native)
  {
    
  }
  control_variable           (const control_variable&  that) = delete;
  control_variable           (      control_variable&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_CVAR_HANDLE_NULL;
  }
  virtual ~control_variable  ()
  {
    if (managed_ && native_ != MPI_T_CVAR_HANDLE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_free, (&native_))
  }
  control_variable& operator=(const control_variable&  that) = delete;
  control_variable& operator=(      control_variable&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_T_CVAR_HANDLE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_T_CVAR_HANDLE_NULL;
    }
    return *this;
  }

  template <typename type>
  type read () const
  {
    type result;
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_read, (native_, static_cast<void*>(&result)))
    return result;
  }
  template <typename type>
  void write(const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_write, (native_, static_cast<const void*>(&value)))
  }

protected:
  bool              managed_ = false;
  MPI_T_cvar_handle native_  = MPI_T_CVAR_HANDLE_NULL;
};

inline std::int32_t                              control_variable_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_num, (&result))
  return result;
}
inline std::vector<control_variable_information> control_variable_info ()
{
  const auto count = control_variable_count();

  std::vector<control_variable_information> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(i);

  return result;
}
}