#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class type_class : std::int32_t
{
  real    = MPI_TYPECLASS_REAL   ,
  integer = MPI_TYPECLASS_INTEGER,
  complex = MPI_TYPECLASS_COMPLEX
};
}