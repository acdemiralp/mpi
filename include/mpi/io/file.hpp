#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class access_mode : std::int32_t
{
  read_only       = MPI_MODE_RDONLY         ,
  read_write      = MPI_MODE_RDWR           ,
  write_only      = MPI_MODE_WRONLY         ,
  create          = MPI_MODE_CREATE         ,
  error_if_exists = MPI_MODE_EXCL           ,
  delete_on_close = MPI_MODE_DELETE_ON_CLOSE,
  unique_open     = MPI_MODE_UNIQUE_OPEN    ,
  append          = MPI_MODE_APPEND         ,
  sequential      = MPI_MODE_SEQUENTIAL     
};

class file
{
public:
  file           ()
  {

  }
  file           (const file&  that) = delete;
  file           (      file&& temp) noexcept
  {
    
  }
  virtual ~file  ()
  {
    
  }
  file& operator=(const file&  that) = delete;
  file& operator=(      file&& temp) noexcept
  {
    
  }

protected:
  MPI_File native_;
};
}