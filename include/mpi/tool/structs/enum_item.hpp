#pragma once

#include <cstdint>
#include <string>

namespace mpi::tool
{
struct enum_item
{
  std::int32_t index;
  std::int32_t value;
  std::string  name ;
};
}