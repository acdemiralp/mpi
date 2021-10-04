#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
using address = MPI_Aint;

inline address get_address(const void* location)
{
  address result;
  MPI_CHECK_ERROR_CODE(MPI_Get_address, (location, &result))
  return  result;
}
inline address add        (const address& lhs, const address& rhs)
{
  return MPI_Aint_add (lhs, rhs);
}
inline address subtract   (const address& lhs, const address& rhs)
{
  return MPI_Aint_diff(lhs, rhs);
}
}