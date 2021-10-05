#pragma once

#include <cstdint>

namespace mpi
{
struct dimension
{
  std::int32_t size    ;
  bool         periodic;
};
}