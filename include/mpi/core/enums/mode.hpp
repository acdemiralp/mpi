#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class mode : std::int32_t
{
  no_check   = MPI_MODE_NOCHECK  ,
  no_store   = MPI_MODE_NOSTORE  ,
  no_put     = MPI_MODE_NOPUT    ,
  no_precede = MPI_MODE_NOPRECEDE,
  no_succeed = MPI_MODE_NOSUCCEED
};
}