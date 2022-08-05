#pragma once

#include <cstdint>

#include <mpi/core/type/data_type.hpp>
#include <mpi/io/data_representation.hpp>

namespace mpi::io
{
struct file_view
{
  offset              displacement    = MPI_DISPLACEMENT_CURRENT; // The default is only valid if the file access mode is sequential.
  data_type           elementary_type = data_type(MPI_DATATYPE_NULL, true);
  data_type           file_type       = data_type(MPI_DATATYPE_NULL, true);
  data_representation representation  = data_representation();
};
}