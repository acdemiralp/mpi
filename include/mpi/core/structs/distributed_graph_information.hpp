#pragma once

#include <cstdint>

namespace mpi
{
struct distributed_graph_information
{
  std::int32_t in_degree ;
  std::int32_t out_degree;
  bool         weighted  ;
};
}