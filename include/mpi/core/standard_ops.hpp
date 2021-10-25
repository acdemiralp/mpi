#pragma once

#include <mpi/core/op.hpp>

namespace mpi::ops
{
inline const op sum                  (MPI_SUM    );
inline const op product              (MPI_PROD   );
inline const op minimum              (MPI_MIN    );
inline const op minimum_with_location(MPI_MINLOC );
inline const op maximum              (MPI_MAX    );
inline const op maximum_with_location(MPI_MAXLOC );
inline const op bitwise_and          (MPI_BAND   );
inline const op bitwise_or           (MPI_BOR    );
inline const op bitwise_xor          (MPI_BXOR   );
inline const op logical_and          (MPI_LAND   );
inline const op logical_or           (MPI_LOR    );
inline const op logical_xor          (MPI_LXOR   );
inline const op replace              (MPI_REPLACE);
inline const op no_op                (MPI_NO_OP  );
}