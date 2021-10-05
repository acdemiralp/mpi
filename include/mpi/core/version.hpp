#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
inline std::array<std::int32_t, 2> version        ()
{
  std::array<std::int32_t, 2> result {};
  MPI_CHECK_ERROR_CODE(MPI_Get_version, (&result[0], &result[1]))
  return result;
}
inline std::string                 library_version()
{
  std::string  result(MPI_MAX_LIBRARY_VERSION_STRING, ' ');
  std::int32_t size  (0);
  MPI_CHECK_ERROR_CODE(MPI_Get_library_version, (&result[0], &size))
  result.resize(size);
  return result;
}
}