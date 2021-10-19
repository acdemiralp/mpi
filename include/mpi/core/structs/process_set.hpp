#pragma once

#include <cstdint>
#include <string>

namespace mpi
{
struct process_set
{
  std::string  name;
  std::int32_t size;
};
}