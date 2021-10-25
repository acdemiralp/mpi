#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class distribution : std::int32_t
{
  block  = MPI_DISTRIBUTE_BLOCK ,
  cyclic = MPI_DISTRIBUTE_CYCLIC,
  none   = MPI_DISTRIBUTE_NONE
};
}