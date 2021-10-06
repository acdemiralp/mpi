#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
enum class scope : std::int32_t
{
  constant    = MPI_T_SCOPE_CONSTANT,
  read_only   = MPI_T_SCOPE_READONLY,
  local       = MPI_T_SCOPE_LOCAL   ,
  group       = MPI_T_SCOPE_GROUP   ,
  group_equal = MPI_T_SCOPE_GROUP_EQ,
  all         = MPI_T_SCOPE_ALL     ,
  all_equal   = MPI_T_SCOPE_ALL_EQ
};
}