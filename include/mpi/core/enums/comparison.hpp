#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class comparison : std::int32_t
{
  identical = MPI_IDENT    ,
  congruent = MPI_CONGRUENT,
  similar   = MPI_SIMILAR  ,
  unequal   = MPI_UNEQUAL
};
}