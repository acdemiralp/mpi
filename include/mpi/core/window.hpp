#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include <mpi/core/enums/lock_type.hpp>
#include <mpi/core/enums/mode.hpp>
#include <mpi/core/error/error_handler.hpp>
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
  explicit window  ()
  : managed_(true)
  {
    // Apply the difference in constructor.
    // MPI_CHECK_ERROR_CODE(MPI_Win_allocate, ())
    // MPI_CHECK_ERROR_CODE(MPI_Win_create  , ())
  }
  explicit window  (const MPI_Win  native)
  : native_(native)
  {
    
  }
  window           (const window&  that) = delete;
  window           (      window&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_WIN_NULL;
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

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_WIN_NULL;
    }
    return *this;
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

  [[nodiscard]]
  group                group             () const
  {
    MPI_Group result;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_group, (native_, &result))
    return mpi::group(result);
  }

  void                 lock              (const lock_type type, const std::int32_t rank, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_lock      , (static_cast<std::int32_t>(type), rank, assert ? static_cast<std::int32_t>(*assert) : 0, native_))
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

  void                 fence             (const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_fence, (assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 synchronize       () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_sync, (native_))
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

  [[nodiscard]]
  bool                 managed          () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Win              native           () const
  {
    return native_;
  }

protected:
  bool    managed_ = false;
  MPI_Win native_  = MPI_WIN_NULL;
};
}