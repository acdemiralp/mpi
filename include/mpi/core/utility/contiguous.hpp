#pragma once

#include <vector>

#include <mpi/core/utility/sequential_container_traits.hpp>

namespace mpi
{
// MPI requires data to be provided as a pointer to contiguous memory.
// Basic types and contiguous sequential containers naturally satisfy this requirement, while associative and non-contiguous sequential containers do not.
// This helper function converts any container to a contiguous sequential one (by default a vector).
template <typename type, contiguous_sequential_container contiguous_type = std::vector<typename type::value_type>>
contiguous_type make_contiguous(const type& container)
{
  return contiguous_type(container.begin(), container.end());
}
}