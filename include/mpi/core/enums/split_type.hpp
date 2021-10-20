#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class split_type
{
  shared          = MPI_COMM_TYPE_SHARED   
#ifdef MPI_USE_LATEST
                                           ,
  hardware_guided = MPI_COMM_TYPE_HW_GUIDED
#endif
};
}