#pragma once

#include <cstdint>
#include <vector>

#include <mpi/core/enums/distribution.hpp>

namespace mpi
{
struct distributed_array_information
{
  std::int32_t              size                  ;
  std::int32_t              rank                  ;
  std::vector<std::int32_t> global_sizes          ;
  std::vector<distribution> distributions         ;
  std::vector<std::int32_t> distribution_arguments;
  std::vector<std::int32_t> process_grid_sizes    ;
  bool                      fortran_order         = false;
};

inline constexpr std::int32_t distribution_default_arguments(MPI_DISTRIBUTE_DFLT_DARG);
}