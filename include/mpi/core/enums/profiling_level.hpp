#pragma once

#include <cstdint>

namespace mpi
{
enum class profiling_level : std::int32_t
{
  disabled = 0,
  enabled  = 1,
  flush    = 2
};
}