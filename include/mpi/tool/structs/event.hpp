#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
#ifdef MPI_USE_LATEST
struct event
{
  
};

inline std::int32_t event_source_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_source_get_num, (&result))
  return result;
}
#endif
}