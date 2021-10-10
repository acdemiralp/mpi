#pragma once

#include <cstdint>
#include <vector>

#include <mpi/core/enums/combiner.hpp>

namespace mpi
{
struct data_type_information
{
  std::vector<std::int32_t> integers  ;
  std::vector<std::int64_t> addresses ;
  std::vector<std::int32_t> data_types;
  combiner                  combiner  ;
};
}