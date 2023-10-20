#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
constexpr std::int32_t version     = MPI_VERSION   ;
constexpr std::int32_t sub_version = MPI_SUBVERSION;

inline std::array<std::int32_t, 2> get_version        ()
{
  std::array<std::int32_t, 2> result {};
  MPI_CHECK_ERROR_CODE(MPI_Get_version, (&result[0], &result[1]))
  return result;
}
inline std::string                 get_library_version()
{
  std::string  result(MPI_MAX_LIBRARY_VERSION_STRING, '\n');
  std::int32_t size  (0);
  MPI_CHECK_ERROR_CODE(MPI_Get_library_version, (result.data(), &size))
  result.resize(static_cast<std::size_t>(size));
  return result;
}
}