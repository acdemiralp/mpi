#pragma once

#include <array>
#include <cstddef>
#include <tuple>

#include <mpi/core/type/compliant_traits.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
// Compound type consisting of the buffered send overhead and a compliant type for use with MPI_Buffer_attach.
template <compliant type>
struct overhead_type
{
  std::array<std::byte, MPI_BSEND_OVERHEAD> overhead;
  type                                      value   ;
};

// Example usage: mpi::attach_buffer<mpi::buffer_type<char[7], double[2], char[17]>>();
template <compliant... types>
using buffer_type = std::tuple<overhead_type<types>...>;
}