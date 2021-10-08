#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class lock_type : std::int32_t
{
  exclusive = MPI_LOCK_EXCLUSIVE,
  shared    = MPI_LOCK_SHARED
};
}