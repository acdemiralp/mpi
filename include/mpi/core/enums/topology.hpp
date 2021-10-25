#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class topology : std::int32_t
{
  graph             = MPI_GRAPH     ,
  cartesian         = MPI_CART      ,
  distributed_graph = MPI_DIST_GRAPH,
  undefined         = MPI_UNDEFINED
};
}