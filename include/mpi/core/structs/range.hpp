#pragma once

#include <cstdint>

namespace mpi
{
struct range
{
  std::int32_t first ;
  std::int32_t last  ;
  std::int32_t stride;
};
}