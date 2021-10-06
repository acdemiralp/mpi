#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace mpi
{
struct adjacent_distributed_graph
{
  std::vector  <std::int32_t>              sources            ;
  std::optional<std::vector<std::int32_t>> source_weights     ;
  std::vector  <std::int32_t>              destinations       ;
  std::optional<std::vector<std::int32_t>> destination_weights;
};
}