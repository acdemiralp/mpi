#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class thread_support : std::int32_t
{
  single     = MPI_THREAD_SINGLE    ,
  funneled   = MPI_THREAD_FUNNELED  ,
  serialized = MPI_THREAD_SERIALIZED,
  multiple   = MPI_THREAD_MULTIPLE
};
}