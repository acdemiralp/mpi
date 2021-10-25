#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace mpi
{
struct distributed_graph
{
  std::vector  <std::int32_t>              sources     ;
  std::vector  <std::int32_t>              degrees     ;
  std::vector  <std::int32_t>              destinations;
  std::optional<std::vector<std::int32_t>> weights     ;
};
}