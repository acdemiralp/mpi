#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
#ifdef MPI_USE_LATEST
enum class window_flavor : std::int32_t
{
  create   = MPI_WIN_CREATE         ,
  allocate = MPI_WIN_ALLOCATE       ,
  dynamic  = MPI_WIN_CREATE_DYNAMIC ,
  shared   = MPI_WIN_ALLOCATE_SHARED
};
#endif
}