#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

// There is no reason to prefer the following over native functionality. They are included for completeness.
namespace mpi
{
template <typename type = void>
type*       allocate   (const aint size, const information& info = information())
{
  type* result {};
  MPI_CHECK_ERROR_CODE(MPI_Alloc_mem, (size, info.native(), static_cast<void*>(result)))
  return result;
}
template <typename type = void>
void        free       (type* location)
{
  MPI_CHECK_ERROR_CODE(MPI_Free_mem , (static_cast<void*>(location)))
}

template <typename type = void>
aint        get_address(const type* location)
{
  aint result;
  MPI_CHECK_ERROR_CODE(MPI_Get_address, (static_cast<const void*>(location), &result))
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