#pragma once

#include <string>
#include <vector>

#include <mpi/tool/structs/enum_item.hpp>

namespace mpi::tool
{
struct enum_information
{
  std::string            name ;
  std::vector<enum_item> items;
};
}