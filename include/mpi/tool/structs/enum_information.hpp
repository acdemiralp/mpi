#pragma once

#include <string>
#include <vector>

#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/enum_item.hpp>

namespace mpi::tool
{
struct enum_information
{
  MPI_T_enum             type ;
  std::string            name ;
  std::vector<enum_item> items;
};
}