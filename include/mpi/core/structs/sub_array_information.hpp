#pragma once

#include <cstdint>
#include <vector>

namespace mpi
{
struct sub_array_information
{
  std::vector<std::int32_t> sizes         ;
  std::vector<std::int32_t> sub_sizes     ;
  std::vector<std::int32_t> starts        ;
  bool                      fortran_order = false;
};
}