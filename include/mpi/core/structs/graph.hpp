#pragma once

#include <cstdint>
#include <vector>

namespace mpi
{
struct graph
{
  std::vector<std::int32_t> degrees;
  std::vector<std::int32_t> edges  ;
};
}