#pragma once

#include <mpi.h>

#if MPI_VERSION == 4 && MPI_SUBVERSION >= 0
#define MPI_USE_LATEST
#endif

namespace mpi
{
using aint   = MPI_Aint ;
using count  = MPI_Count;
using offset = MPI_Offset;
}