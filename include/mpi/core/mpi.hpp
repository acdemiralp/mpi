#pragma once

#include <mpi.h>

namespace mpi
{
inline double wall_clock_time     ()
{
  return MPI_Wtime();
}
inline double wall_clock_tick_rate()
{
  return MPI_Wtick();
}
}