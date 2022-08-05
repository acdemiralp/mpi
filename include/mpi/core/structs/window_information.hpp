#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
struct window_information
{
  void*        base        ;
  aint         size        ;
  std::int32_t displacement;
};
}