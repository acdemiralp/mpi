#pragma once

#include <cstdint>

namespace mpi
{
struct window_information
{
  void*        base        ;
  std::int64_t size        ;
  std::int32_t displacement;
};
}