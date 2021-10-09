#pragma once

#include <cstdint>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/io/enums/access_mode.hpp>

namespace mpi
{
class file
{
public:
  explicit file  (const communicator& communicator, const std::string& filepath, const access_mode access_mode, const information& information = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_File_open, (communicator.native(), filepath.c_str(), static_cast<std::int32_t>(access_mode), information.native(), &native_))
  }
  explicit file  (MPI_File native)
  : native_(native)
  {

  }
  file           (const file&  that) = delete;
  file           (      file&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_FILE_NULL;
  }
  virtual ~file  ()
  {
    if (managed_ && native_ != MPI_FILE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_File_close, (&native_))
  }
  file& operator=(const file&  that) = delete;
  file& operator=(      file&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_FILE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_File_close, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_FILE_NULL;
    }
    return *this;
  }

  [[nodiscard]]
  information        information       () const
  {
    MPI_Info result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_info, (native_, &result))
    return mpi::information(result);
  }
  void               set_information   (const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_info, (native_, value.native()))
  }

  [[nodiscard]]
  MPI_Offset         size              () const
  {
    MPI_Offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_size, (native_, &result))
    return result;
  }
  void               set_size          (const MPI_Offset value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_size, (native_, value))
  }

  [[nodiscard]]
  bool               atomic            () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_atomicity, (native_, &result))
    return static_cast<bool>(result);
  }
  void               set_atomic        (const bool value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_atomicity, (native_, static_cast<std::int32_t>(value)))
  }

  [[nodiscard]]
  access_mode        access_mode       () const
  {
    mpi::access_mode result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_amode, (native_, reinterpret_cast<std::int32_t*>(&result)))
    return result;
  }
  [[nodiscard]]
  group              group             () const
  {
    MPI_Group result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_group, (native_, &result))
    return mpi::group(result);
  }

  [[nodiscard]]
  MPI_Offset         position          () const
  {
    MPI_Offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_position       , (native_, &result))
    return result;
  }
  [[nodiscard]]
  MPI_Offset         shared_position   () const
  {
    MPI_Offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_position_shared, (native_, &result))
    return result;
  }

  [[nodiscard]]
  MPI_Offset         byte_offset       (const MPI_Offset offset) const
  {
    MPI_Offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_byte_offset    , (native_, offset, &result))
    return result;
  }

  [[nodiscard]]
  file_error_handler error_handler     () const
  {
    MPI_Errhandler result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_errhandler, (native_, &result))
    return file_error_handler(result);
  }
  void               set_error_handler (const file_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_errhandler, (native_, value.native()))
  }
  void               call_error_handler(const error_code& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_call_errhandler, (native_, value.native()))
  }

  void               synchronize       () const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_sync, (native_))
  }

protected:
  bool     managed_ = false;
  MPI_File native_  = MPI_FILE_NULL;
};
}