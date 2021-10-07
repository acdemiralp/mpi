#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
enum class performance_variable_type : std::int32_t
{
  state          = MPI_T_PVAR_CLASS_STATE        ,
  level          = MPI_T_PVAR_CLASS_LEVEL        ,
  size           = MPI_T_PVAR_CLASS_SIZE         ,
  percentage     = MPI_T_PVAR_CLASS_PERCENTAGE   ,
  high_watermark = MPI_T_PVAR_CLASS_HIGHWATERMARK,
  low_watermark  = MPI_T_PVAR_CLASS_LOWWATERMARK ,
  counter        = MPI_T_PVAR_CLASS_COUNTER      ,
  aggregate      = MPI_T_PVAR_CLASS_AGGREGATE    ,
  timer          = MPI_T_PVAR_CLASS_TIMER        ,
  generic        = MPI_T_PVAR_CLASS_GENERIC
};
}