#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include <mpi/core/enums/lock_type.hpp>
#include <mpi/core/enums/mode.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class window_key_value
{
public:
  using copy_function   = std::int32_t (*) (MPI_Win, std::int32_t, void*, void*, void*, std::int32_t*);
  using delete_function = std::int32_t (*) (MPI_Win, std::int32_t, void*, void*);

  explicit window_key_value  (
    const copy_function   copy_function   = MPI_WIN_DUP_FN        , 
    const delete_function delete_function = MPI_WIN_NULL_DELETE_FN, 
    void*                 extra_state     = nullptr)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create_keyval, (copy_function, delete_function, &native_, extra_state)) // Extra state is unused.
  }
  window_key_value           (const window_key_value&  that) = delete;
  window_key_value           (      window_key_value&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_KEYVAL_INVALID;
  }
  virtual ~window_key_value  ()
  {
    if (managed_ && native_ != MPI_KEYVAL_INVALID)
      MPI_CHECK_ERROR_CODE(MPI_Win_free_keyval, (&native_))
  }
  window_key_value& operator=(const window_key_value&  that) = delete;
  window_key_value& operator=(      window_key_value&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_KEYVAL_INVALID)
        MPI_CHECK_ERROR_CODE(MPI_Win_free_keyval, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_KEYVAL_INVALID;
    }
    return *this;
  }

  [[nodiscard]]
  bool         managed() const
  {
    return managed_;
  }
  [[nodiscard]]
  std::int32_t native () const
  {
    return native_;
  }

protected:
  bool         managed_ = false;
  std::int32_t native_  = MPI_KEYVAL_INVALID;
};

class window
{
public:
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
  std::string name           () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, ' ');
    std::int32_t length(0);
    MPI_CHECK_ERROR_CODE(MPI_Win_get_name, (native_, &result[0], &length))
    result.resize(static_cast<std::size_t>(length));
    return result;
  }
  void        set_name       (const std::string&      value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_name, (native_, value.c_str()))
  }

  [[nodiscard]]
  information information    () const
  {
    MPI_Info result;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_info, (native_, &result))
    return mpi::information(result);
  }
  void        set_information(const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_info, (native_, value.native()))
  }

  [[nodiscard]]
  group       group          () const
  {
    MPI_Group result;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_group, (native_, &result))
    return mpi::group(result);
  }

  void        lock           (const lock_type type, const std::int32_t rank, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_lock      , (static_cast<std::int32_t>(type), rank, assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void        lock_all       (const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_lock_all  , (assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void        unlock         (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_unlock    , (rank, native_))
  }
  void        unlock_all     () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_unlock_all, (native_))
  }

  void        flush          (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush          , (rank, native_))
  }
  void        flush_all      () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_all      , (      native_))
  }
  void        flush_local    (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_local    , (rank, native_))
  }
  void        flush_local_all() const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_local_all, (      native_))
  }

  void        fence          (const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_fence, (assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void        synchronize    () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_sync, (native_))
  }

  [[nodiscard]]
  bool        test           () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Win_test, (native_, &result))
    return static_cast<bool>(result);
  }
  void        wait           () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_wait, (native_))
  }

  void        post           (const mpi::group& group, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_post    , (group.native(), assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void        start          (const mpi::group& group, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_start   , (group.native(), assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void        complete       () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_complete, (native_))
  }

  [[nodiscard]]
  bool        managed        () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Win     native         () const
  {
    return native_;
  }

protected:
  bool    managed_ = false;
  MPI_Win native_  = MPI_WIN_NULL;
};
}