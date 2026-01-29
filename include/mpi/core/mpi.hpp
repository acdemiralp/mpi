#pragma once

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1
#include <mpi.h>

#if MPI_VERSION >= 4 && MPI_SUBVERSION >= 0
#define MPI_GEQ_4_0
#endif

namespace mpi
{
using aint   = MPI_Aint  ;
using count  = MPI_Count ;
using offset = MPI_Offset;
}