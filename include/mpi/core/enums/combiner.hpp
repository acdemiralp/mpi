#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class combiner : std::int32_t
{
  named                       = MPI_COMBINER_NAMED         ,
  duplicate                   = MPI_COMBINER_DUP           ,
  contiguous                  = MPI_COMBINER_CONTIGUOUS    ,
  vector                      = MPI_COMBINER_VECTOR        ,
  heterogeneous_vector        = MPI_COMBINER_HVECTOR       ,
  indexed                     = MPI_COMBINER_INDEXED       ,
  heterogeneous_indexed       = MPI_COMBINER_HINDEXED      ,
  indexed_block               = MPI_COMBINER_INDEXED_BLOCK ,
  heterogeneous_indexed_block = MPI_COMBINER_HINDEXED_BLOCK,
  structure                   = MPI_COMBINER_STRUCT        ,
  sub_array                   = MPI_COMBINER_SUBARRAY      ,
  distributed_array           = MPI_COMBINER_DARRAY        ,
  f90_real                    = MPI_COMBINER_F90_REAL      ,
  f90_complex                 = MPI_COMBINER_F90_COMPLEX   ,
  f90_integer                 = MPI_COMBINER_F90_INTEGER   ,
  resized                     = MPI_COMBINER_RESIZED
};
}