#pragma once

#include <cstdint>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/type/data_type.hpp>
#include <mpi/core/type/type_traits.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/io/enums/access_mode.hpp>

namespace mpi::io
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
  access_mode        access_mode       () const
  {
    io::access_mode result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_amode, (native_, reinterpret_cast<std::int32_t*>(&result)))
    return result;
  }
  [[nodiscard]]
  std::int64_t       byte_offset       (const std::int64_t offset) const
  {
    std::int64_t result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_byte_offset    , (native_, offset, &result))
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
  std::int64_t       position          () const
  {
    std::int64_t result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_position       , (native_, &result))
    return result;
  }
  [[nodiscard]]
  std::int64_t       shared_position   () const
  {
    std::int64_t result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_position_shared, (native_, &result))
    return result;
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
  std::int64_t       size              () const
  {
    std::int64_t result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_size, (native_, &result))
    return result;
  }
  void               set_size          (const std::int64_t value) const
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
    MPI_CHECK_ERROR_CODE(MPI_File_set_atomicity, (native_, value))
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

  // The type and data_type must match. It is possible to automate this by implementing get_data_type<type> to map type to a data_type (see below).
  template <typename type>
  std::pair<std::vector<type>, status> read    (const std::int32_t count, const data_type& data_type)
  {
    std::pair<std::vector<type>, status> result {std::vector<type>(count), {}};
    MPI_CHECK_ERROR_CODE(MPI_File_read    , (native_, static_cast<void*>(result.first.data()), count, data_type.native(), &result.second))
    return result;
  }
  template <typename type>
  std::pair<std::vector<type>, status> read_all(const std::int32_t count, const data_type& data_type)
  {
    std::pair<std::vector<type>, status> result {std::vector<type>(count), {}};
    MPI_CHECK_ERROR_CODE(MPI_File_read_all, (native_, static_cast<void*>(result.first.data()), count, data_type.native(), &result.second))
    return result;
  }

  template <typename type>
  std::pair<std::vector<type>, status> read    (const std::int32_t count)
  {
    return read    <type>(count, type_traits<type>::get_data_type());
  }
  template <typename type>
  std::pair<std::vector<type>, status> read_all(const std::int32_t count)
  {
    return read_all<type>(count, type_traits<type>::get_data_type());
  }


  [[nodiscard]]
  bool               managed           () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_File           native            () const
  {
    return native_;
  }

protected:
  bool     managed_ = false;
  MPI_File native_  = MPI_FILE_NULL;
};

inline void delete_file(const std::string& filepath, const information& information = mpi::information())
{
  MPI_CHECK_ERROR_CODE(MPI_File_delete, (filepath.c_str(), information.native()))
}
}