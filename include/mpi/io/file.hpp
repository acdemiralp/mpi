#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/io/enums/access_mode.hpp>

namespace mpi
{
class file
{
public:
  explicit file  (const MPI_File native)
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
  std::int64_t size() const
  {
    MPI_Offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_size, (native_, &result))
    return result;
  }

protected:
  bool     managed_ = false;
  MPI_File native_  = MPI_FILE_NULL;
};
}