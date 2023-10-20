#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>

#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/performance_variable.hpp>
#include <mpi/tool/utility/object_variant.hpp>
#include <mpi/tool/session.hpp>

namespace mpi::tool
{
class performance_variable_handle
{
public:
  explicit performance_variable_handle  (const performance_variable& variable, const session& session)
  : managed_(true), session_(session)
  {
    if      (variable.bind_type == bind_type::communicator)
    {
      MPI_Comm handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = communicator(handle); // Unmanaged construction.
    }
    else if (variable.bind_type == bind_type::data_type)
    {
      MPI_Datatype handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = data_type(handle);
    }
    else if (variable.bind_type == bind_type::error_handler)
    {
      MPI_Errhandler handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = error_handler(handle);
    }
    else if (variable.bind_type == bind_type::file)
    {
      MPI_File handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = io::file(handle);
    }
    else if (variable.bind_type == bind_type::group)
    {
      MPI_Group handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = group(handle);
    }
    else if (variable.bind_type == bind_type::information)
    {
      MPI_Info handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = information(handle);
    }
    else if (variable.bind_type == bind_type::message)
    {
      MPI_Message handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = message(handle);
    }
    else if (variable.bind_type == bind_type::op)
    {
      MPI_Op handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = op(handle);
    }
    else if (variable.bind_type == bind_type::request)
    {
      MPI_Request handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = request(handle);
    }
    else if (variable.bind_type == bind_type::window)
    {
      MPI_Win handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = window(handle);
    }
#ifdef MPI_USE_UNSUPPORTED
    else if (variable.bind_type == bind_type::session)
    {
      MPI_Session handle;
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_alloc, (session_.native(), variable.index, &handle, &native_, &count_))
      object_ = mpi::session(handle);
    }
#endif
  }
  explicit performance_variable_handle  (const MPI_T_pvar_handle&    native  , const session& session, const bool managed = false, const std::int32_t count = 1, std::optional<object_variant> object = std::nullopt)
  : managed_(managed), native_(native), count_(count), object_(std::move(object)), session_(session)
  {
    
  }
  performance_variable_handle           (const performance_variable_handle&  that) = delete;
  performance_variable_handle           (      performance_variable_handle&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_), count_(temp.count_), object_(std::move(temp.object_)), session_(temp.session_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_PVAR_HANDLE_NULL;
    temp.count_   = 1;
    temp.object_  = std::nullopt;
  }
  virtual ~performance_variable_handle  () noexcept(false)
  {
    if (managed_ && native_ != MPI_T_PVAR_HANDLE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_free, (session_.native(), &native_))
  }
  performance_variable_handle& operator=(const performance_variable_handle&  that) = delete;
  performance_variable_handle& operator=(      performance_variable_handle&& temp) = delete; // Assignment is disabled due to a constant reference member variable.

  template <typename type>
  type                                 read      () const
  {
    type result;
    container_adapter<type>::resize(result, count_);
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_read     , (session_.native(), native_, container_adapter<type>::data(result)))
    return result;
  }
  template <typename type>
  type                                 read_reset() const
  {
    type result;
    container_adapter<type>::resize(result, count_);
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_readreset, (session_.native(), native_, container_adapter<type>::data(result)))
    return result;
  }
  template <typename type>
  void                                 write     (const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_write    , (session_.native(), native_, container_adapter<type>::data(value)))
  }

  void                                 start     () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_start, (session_.native(), native_))
  }
  void                                 stop      () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_stop , (session_.native(), native_))
  }
  void                                 reset     () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_reset, (session_.native(), native_))
  }

  [[nodiscard]]
  bool                                 managed   () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_T_pvar_handle                    native    () const
  {
    return native_;
  }
  [[nodiscard]]
  std::int32_t                         count     () const
  {
    return count_;
  }
  [[nodiscard]]
  const std::optional<object_variant>& object    () const
  {
    return object_;
  }
  [[nodiscard]]
  const tool::session&                 session   () const
  {
    return session_;
  }

protected:
  bool                          managed_ = false;
  MPI_T_pvar_handle             native_  = MPI_T_PVAR_HANDLE_NULL;
  std::int32_t                  count_   = 1;
  std::optional<object_variant> object_  = std::nullopt;
  const tool::session&          session_ ;
};
}