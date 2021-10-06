#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <mpi/core/information.hpp>

namespace mpi
{
struct spawn_data
{
  std::string              command      ;
  std::vector<std::string> arguments    ;
  std::int32_t             process_count;
  information              information  ;
};
}