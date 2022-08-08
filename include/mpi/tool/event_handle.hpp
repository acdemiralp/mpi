#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <utility>

#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/enums/callback_safety.hpp>
#include <mpi/tool/structs/event.hpp>
#include <mpi/tool/utility/object_variant.hpp>

namespace mpi::tool
{
#ifdef MPI_USE_LATEST
class event_handle
{
public:
  using callback_type        = void (*) (MPI_T_event_instance, MPI_T_event_registration,               MPI_T_cb_safety, void*);
  using dropped_handler_type = void (*) (count               , MPI_T_event_registration, std::int32_t, MPI_T_cb_safety, void*);
  using callback_map_type    = std::unordered_map<callback_safety, std::function<void(const event_instance&, const event_handle&, callback_safety)>>;

  explicit event_handle  (const event& event, const information& information = mpi::information())
  : managed_(true), copy_buffer_size_(event.displacements.back() + event.data_types.back().size())
  {
    if (event.bind_type == bind_type::file)
    {
      MPI_File handle;
      MPI_CHECK_ERROR_CODE(MPI_T_event_handle_alloc, (event.index, &handle, information.native(), &native_))

      // Unmanaged construction.
      object_ = io::file(handle);
    }
    else
    {
      std::int32_t handle; // Abusing the fact that all native MPI object handles are std::int32_ts.
      MPI_CHECK_ERROR_CODE(MPI_T_event_handle_alloc, (event.index, &handle, information.native(), &native_))

      // Unmanaged construction.
      if      (event.bind_type == bind_type::communicator ) object_ = communicator    (handle);
      else if (event.bind_type == bind_type::data_type    ) object_ = data_type       (handle);
      else if (event.bind_type == bind_type::error_handler) object_ = error_handler   (handle);
      else if (event.bind_type == bind_type::group        ) object_ = group           (handle);
      else if (event.bind_type == bind_type::information  ) object_ = mpi::information(handle);
      else if (event.bind_type == bind_type::message      ) object_ = message         (handle);
      else if (event.bind_type == bind_type::op           ) object_ = op              (handle);
      else if (event.bind_type == bind_type::request      ) object_ = request         (handle);
      else if (event.bind_type == bind_type::window       ) object_ = window          (handle);
      else if (event.bind_type == bind_type::session      ) object_ = session         (handle);
    }
  }
  explicit event_handle  (const MPI_T_event_registration native, const bool managed = false, std::optional<object_variant> object = std::nullopt)
  : managed_(managed), native_(native), object_(std::move(object))
  {

  }
  event_handle           (const event_handle&  that) = delete;
  event_handle           (      event_handle&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_), object_(std::move(temp.object_)), callback_map_(std::move(temp.callback_map_)), copy_buffer_size_(temp.copy_buffer_size_)
  {
    temp.managed_          = false;
    temp.native_           = MPI_T_EVENT_HANDLE_NULL;
    temp.object_           = std::nullopt;
    temp.callback_map_     .clear();
    temp.copy_buffer_size_ = 0;
  }
  virtual ~event_handle  () noexcept(false)
  {
    if (managed_ && native_ != MPI_T_EVENT_HANDLE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_T_event_handle_free, (native_, nullptr, nullptr)) // Destructor cannot have parameters, hence pass nullptr to the callback and user data.
  }
  event_handle& operator=(const event_handle&  that) = delete;
  event_handle& operator=(      event_handle&& temp) noexcept(false)
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_T_EVENT_HANDLE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_T_event_handle_free, (native_, nullptr, nullptr))

      managed_               = temp.managed_;
      native_                = temp.native_ ;
      object_                = std::move(temp.object_);
      callback_map_          = std::move(temp.callback_map_);
      copy_buffer_size_      = temp.copy_buffer_size_;
                        
      temp.managed_          = false;
      temp.native_           = MPI_T_EVENT_HANDLE_NULL;
      temp.object_           = std::nullopt;
      temp.callback_map_     .clear();
      temp.copy_buffer_size_ = 0;
    }
    return *this;
  }

  [[nodiscard]]                                                           
  information                          information             () const
  {
    mpi::information result(MPI_INFO_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_T_event_handle_get_info, (native_, &result.native_))
    return result;
  }
  void                                 set_information         (const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_event_handle_set_info, (native_, value.native()))
  }

  [[nodiscard]]                                                           
  mpi::information                     callback_information    (const callback_safety safety) const
  {
    mpi::information result(MPI_INFO_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_T_event_callback_get_info, (native_, static_cast<MPI_T_cb_safety>(safety), &result.native_))
    return result;
  }
  void                                 set_callback_information(const callback_safety safety, const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_event_callback_set_info, (native_, static_cast<MPI_T_cb_safety>(safety), value.native()))
  }

  void                                 set_dropped_handler     (const dropped_handler_type function) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_event_set_dropped_handler, (native_, function))
  }
  void                                 register_callback       (const callback_safety safety, const callback_type function, const mpi::information& information = mpi::information(), void* user_data = nullptr)
  {
    MPI_CHECK_ERROR_CODE(MPI_T_event_register_callback  , (native_, static_cast<MPI_T_cb_safety>(safety), information.native(), user_data, function))
  }
  
  // Convenience.
  void                                 register_callback       (const callback_safety safety, const std::function<void(const event_instance&, const event_handle&, callback_safety)>& function, const mpi::information& information = mpi::information())
  {
    callback_map_.emplace(static_cast<callback_safety>(safety), function);
    register_callback(safety, [ ] (MPI_T_event_instance instance, MPI_T_event_registration handle, MPI_T_cb_safety safety, void* user_data)
    {
      static_cast<event_handle*>(user_data)->callback_map_[static_cast<callback_safety>(safety)](event_instance(instance), event_handle(handle), static_cast<callback_safety>(safety));
    }, information, this);
  }

  [[nodiscard]]
  bool                                 managed                 () const
  {
    return managed_;
  }
  [[nodiscard]]                                                
  MPI_T_event_registration             native                  () const
  {
    return native_;
  }
  [[nodiscard]]                                                
  const std::optional<object_variant>& object                  () const
  {
    return object_;
  }
  [[nodiscard]]
  const callback_map_type&             callback_map            () const
  {
    return callback_map_;
  }
  [[nodiscard]]
  std::size_t                          copy_buffer_size        () const
  {
    return copy_buffer_size_;
  }

protected:
  bool                          managed_          = false;
  MPI_T_event_registration      native_           = MPI_T_EVENT_HANDLE_NULL;
  std::optional<object_variant> object_           = std::nullopt;
  callback_map_type             callback_map_     = {};
  std::size_t                   copy_buffer_size_ = 0; // Auxiliary for event interface antics.
};
#endif
}