#pragma once

#include <cstdint>

namespace mpi
{
struct neighbor_counts
{
  std::int32_t source_count     ;
  std::int32_t destination_count;
  bool         weighted         ;
};
}