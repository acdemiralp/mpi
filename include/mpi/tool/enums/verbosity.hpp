#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
enum class verbosity : std::int32_t
{
  user_basic           = MPI_T_VERBOSITY_USER_BASIC   ,
  user_detail          = MPI_T_VERBOSITY_USER_DETAIL  ,
  user_all             = MPI_T_VERBOSITY_USER_ALL     ,
  tuner_basic          = MPI_T_VERBOSITY_TUNER_BASIC  ,
  tuner_detail         = MPI_T_VERBOSITY_TUNER_DETAIL ,
  tuner_all            = MPI_T_VERBOSITY_TUNER_ALL    ,
  mpi_developer_basic  = MPI_T_VERBOSITY_MPIDEV_BASIC ,
  mpi_developer_detail = MPI_T_VERBOSITY_MPIDEV_DETAIL,
  mpi_developer_all    = MPI_T_VERBOSITY_MPIDEV_ALL
};
}