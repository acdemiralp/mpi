#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/control_variable.hpp>
#include <mpi/tool/container_adapter.hpp>
#include <mpi/tool/object_variant.hpp>

namespace mpi::tool
{
class control_variable_handle
{
public:
  explicit control_variable_handle  (const control_variable& variable)
  : managed_(true)
  {
    if (variable.bind_type == bind_type::file)
    {
      MPI_File handle;
      MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_alloc, (variable.index, static_cast<void*>(&handle), &native_, &count_))

      object_ = file(handle);
    }
    else
    {
      std::int32_t handle; // Abusing the fact that all native MPI object handles are std::int32_ts.
      MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_alloc, (variable.index, static_cast<void*>(&handle), &native_, &count_))

      if      (variable.bind_type == bind_type::communicator ) object_ = communicator        (handle);
      else if (variable.bind_type == bind_type::data_type    ) object_ = data_type           (handle);
      else if (variable.bind_type == bind_type::error_handler) object_ = error_handler       (handle);
      else if (variable.bind_type == bind_type::group        ) object_ = group               (handle);
      else if (variable.bind_type == bind_type::information  ) object_ = information         (handle);
      else if (variable.bind_type == bind_type::message      ) object_ = static_cast<message>(handle);
      else if (variable.bind_type == bind_type::op           ) object_ = op                  (handle);
      else if (variable.bind_type == bind_type::request      ) object_ = request             (handle);
      else if (variable.bind_type == bind_type::window       ) object_ = window              (handle);
    }
  }
  explicit control_variable_handle  (const MPI_T_cvar_handle& native, const std::int32_t count = 1, std::optional<object_variant> object = std::nullopt)
  : native_(native), count_(count), object_(std::move(object))
  {

  }
  control_variable_handle           (const control_variable_handle&  that) = delete;
  control_variable_handle           (      control_variable_handle&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_), count_(temp.count_), object_(std::move(temp.object_))
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_CVAR_HANDLE_NULL;
    temp.count_   = 1;
    temp.object_  = std::nullopt;
  }
  virtual ~control_variable_handle  ()
  {
    if (managed_ && native_ != MPI_T_CVAR_HANDLE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_free, (&native_))
  }
  control_variable_handle& operator=(const control_variable_handle&  that) = delete;
  control_variable_handle& operator=(      control_variable_handle&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_T_CVAR_HANDLE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;
      count_        = temp.count_  ;
      object_       = std::move(temp.object_);

      temp.managed_ = false;
      temp.native_  = MPI_T_CVAR_HANDLE_NULL;
      temp.count_   = 1;
      temp.object_  = std::nullopt;
    }
    return *this;
  }

  template <typename type>
  type                                 read       () const
  {
    type result;
    container_adapter<type>::resize(result, count_);
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_read , (native_, container_adapter<type>::data(result)))
    return result;
  }
  template <typename type>
  void                                 write      (const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_write, (native_, container_adapter<type>::data(value)))
  }

  [[nodiscard]]
  bool                                 managed    () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_T_cvar_handle                    native     () const
  {
    return native_;
  }
  [[nodiscard]]
  std::int32_t                         count      () const
  {
    return count_;
  }
  [[nodiscard]]
  const std::optional<object_variant>& object     () const
  {
    return object_;
  }

protected:
  bool                          managed_ = false;
  MPI_T_cvar_handle             native_  = MPI_T_CVAR_HANDLE_NULL;
  std::int32_t                  count_   = 1;
  std::optional<object_variant> object_  = std::nullopt;
};
}