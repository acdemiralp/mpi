#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
template <typename type = void>
type*       allocate   (const aint size, const information& info = information())
{
  type* result {};
  MPI_CHECK_ERROR_CODE(MPI_Alloc_mem, (size, info.native(), result))
  return result;
}
template <typename type = void>
void        free       (type* location)
{
  MPI_CHECK_ERROR_CODE(MPI_Free_mem , (location))
}

template <typename type = void>
aint        get_address(const type* location)
{
  aint result;
  MPI_CHECK_ERROR_CODE(MPI_Get_address, (location, &result))
  return  result;
}

inline aint add        (const aint& lhs, const aint& rhs)
{
  return MPI_Aint_add (lhs, rhs);
}
inline aint difference (const aint& lhs, const aint& rhs)
{
  return MPI_Aint_diff(lhs, rhs);
}
}