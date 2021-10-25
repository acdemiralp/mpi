#pragma once

#include <cstdint>
#include <vector>

#include <mpi/core/enums/combiner.hpp>

namespace mpi
{
class data_type;

struct data_type_information
{
  std::vector<std::int32_t> integers  ;
  std::vector<std::int64_t> addresses ;
  std::vector<data_type>    data_types;
  combiner                  combiner  ;
};
}