#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class window_flavor : std::int32_t
{
  create   = MPI_WIN_FLAVOR_CREATE  ,
  allocate = MPI_WIN_FLAVOR_ALLOCATE,
  dynamic  = MPI_WIN_FLAVOR_DYNAMIC ,
  shared   = MPI_WIN_FLAVOR_SHARED
};
}