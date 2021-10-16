#pragma once

#include <cstdint>
#include <type_traits>

#include <mpi/core/utility/bitset_enum.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
namespace io
{
enum class access_mode : std::int32_t
{
  create          = MPI_MODE_CREATE         ,
  read_only       = MPI_MODE_RDONLY         ,
  write_only      = MPI_MODE_WRONLY         ,
  read_write      = MPI_MODE_RDWR           ,
  delete_on_close = MPI_MODE_DELETE_ON_CLOSE,
  unique_open     = MPI_MODE_UNIQUE_OPEN    ,
  error_if_exists = MPI_MODE_EXCL           ,
  append          = MPI_MODE_APPEND         ,
  sequential      = MPI_MODE_SEQUENTIAL     
};
}

template <>
struct is_bitset_enum<io::access_mode> : std::true_type {};
}