#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class seek_mode : std::int32_t
{
  set     = MPI_SEEK_SET,
  current = MPI_SEEK_CUR,
  end     = MPI_SEEK_END
};
}