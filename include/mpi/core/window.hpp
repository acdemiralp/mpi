#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <mpi/core/enums/mode.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/structs/window_information.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/key_value.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class window
{
public:
  explicit window  (const communicator& communicator, const std::int64_t size, const std::int32_t displacement_unit = 1, const bool shared = false, const information& information = mpi::information())
  : managed_(true)
  {
    if (shared)
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate_shared, (size, displacement_unit, information.native(), communicator.native(), &base_pointer_, &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate       , (size, displacement_unit, information.native(), communicator.native(), &base_pointer_, &native_))
  }
  template <typename type, typename = std::enable_if_t<!std::is_same<type, void>::value>>
  explicit window  (const communicator& communicator, const std::int64_t size,                                           const bool shared = false, const information& information = mpi::information())
  : managed_(true)
  {
    if (shared)
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate_shared, (sizeof(type) * size, sizeof(type), information.native(), communicator.native(), &base_pointer_, &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate       , (sizeof(type) * size, sizeof(type), information.native(), communicator.native(), &base_pointer_, &native_))
  }

  explicit window  (const communicator& communicator, void* base_pointer, const std::int64_t size, const std::int32_t displacement_unit = 1, const information& information = mpi::information())
  : managed_(true), base_pointer_(base_pointer)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create, (base_pointer, size, displacement_unit, information.native(), communicator.native(), &native_))
  }
  template <typename type, typename = std::enable_if_t<!std::is_same<type, void>::value>>
  explicit window  (const communicator& communicator, type* base_pointer, const std::int64_t size,                                           const information& information = mpi::information())
  : managed_(true), base_pointer_(base_pointer)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create, (static_cast<void*>(base_pointer), sizeof(type) * size, sizeof(type), information.native(), communicator.native(), &native_))
  }

  explicit window  (const communicator& communicator, const information& information = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create_dynamic, (information.native(), communicator.native(), &native_))
  }

  explicit window  (const MPI_Win  native, void* base_pointer = nullptr)
  : native_(native), base_pointer_(base_pointer)
  {
    
  }
  window           (const window&  that) = delete;
  window           (      window&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_), base_pointer_(temp.base_pointer_)
  {
    temp.managed_      = false;
    temp.native_       = MPI_WIN_NULL;
    temp.base_pointer_ = nullptr;
  }
  virtual ~window  ()
  {
    if (managed_ && native_ != MPI_WIN_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Win_free, (&native_))
  }
  window& operator=(const window&  that) = delete;
  window& operator=(      window&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_WIN_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Win_free, (&native_))

      managed_           = temp.managed_     ;
      native_            = temp.native_      ;
      base_pointer_      = temp.base_pointer_;

      temp.managed_      = false;
      temp.native_       = MPI_WIN_NULL;
      temp.base_pointer_ = nullptr;
    }
    return *this;
  }

  [[nodiscard]]
  group                group             () const
  {
    MPI_Group result;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_group, (native_, &result))
    return mpi::group(result);
  }
  [[nodiscard]]
  window_information   query_shared      (const std::int32_t rank) const
  {
    window_information result {};
    MPI_CHECK_ERROR_CODE(MPI_Win_shared_query, (native_, rank, &result.size, &result.displacement, &result.base))
    return result;
  }

  [[nodiscard]]
  std::string          name              () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, ' ');
    std::int32_t length(0);
    MPI_CHECK_ERROR_CODE(MPI_Win_get_name, (native_, &result[0], &length))
    result.resize(static_cast<std::size_t>(length));
    return result;
  }
  void                 set_name          (const std::string&      value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_name, (native_, value.c_str()))
  }

  [[nodiscard]]
  information          information       () const
  {
    MPI_Info result;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_info, (native_, &result))
    return mpi::information(result);
  }
  void                 set_information   (const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_info, (native_, value.native()))
  }

  [[nodiscard]]
  window_error_handler error_handler     () const
  {
    MPI_Errhandler result;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_errhandler, (native_, &result))
    return window_error_handler(result);
  }
  void                 set_error_handler (const window_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_errhandler, (native_, value.native()))
  }
  void                 call_error_handler(const error_code& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_call_errhandler, (native_, value.native()))
  }

  template <typename type>
  std::optional<type>  attribute         (const window_key_value& key) const
  {
    type         result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_attr   , (native_, key.native(), static_cast<void*>(&result), &exists))
    return static_cast<bool>(exists) ? result : std::optional<type>(std::nullopt);
  }
  template <typename type>
  void                 set_attribute     (const window_key_value& key, const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_attr   , (native_, key.native(), static_cast<void*>(&value)))
  }
  void                 remove_attribute  (const window_key_value& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_delete_attr, (native_, key.native()))
  }

  void                 attach            (void* value, const std::int64_t size)
  {
    base_pointer_ = value;
    MPI_CHECK_ERROR_CODE(MPI_Win_attach, (native_, value, size))
  }
  template <typename type, typename = std::enable_if_t<!std::is_same<type, void>::value>>
  void                 attach            (type* value, const std::int64_t size)
  {
    base_pointer_ = value;
    MPI_CHECK_ERROR_CODE(MPI_Win_attach, (native_, static_cast<void*>(value), sizeof(type) * size))
  }
  void                 detach            (void* value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_detach, (native_, value))
  }

  void                 post              (const mpi::group& group, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_post    , (group.native(), assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 start             (const mpi::group& group, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_start   , (group.native(), assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 complete          () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_complete, (native_))
  }

  [[nodiscard]]
  bool                 test              () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Win_test, (native_, &result))
    return static_cast<bool>(result);
  }
  void                 wait              () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_wait, (native_))
  }

  void                 fence             (const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_fence, (assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 synchronize       () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_sync, (native_))
  }

  void                 lock              (const std::int32_t rank, const bool shared = false, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_lock      , (shared ? MPI_LOCK_SHARED : MPI_LOCK_EXCLUSIVE, rank, assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 lock_all          (const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_lock_all  , (assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 unlock            (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_unlock    , (rank, native_))
  }
  void                 unlock_all        () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_unlock_all, (native_))
  }

  void                 flush             (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush          , (rank, native_))
  }
  void                 flush_all         () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_all      , (      native_))
  }
  void                 flush_local       (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_local    , (rank, native_))
  }
  void                 flush_local_all   () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_local_all, (      native_))
  }

  [[nodiscard]]
  bool                 managed           () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Win              native            () const
  {
    return native_;
  }
  template <typename type = void>
  type*                base_pointer      () const
  {
    return static_cast<type*>(base_pointer_);
  }

protected:
  bool    managed_      = false;
  MPI_Win native_       = MPI_WIN_NULL;
  void*   base_pointer_ = nullptr;
};
}