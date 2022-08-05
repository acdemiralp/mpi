#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include <mpi/core/structs/graph.hpp>
#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class graph_communicator : public communicator
{
public:
  explicit graph_communicator   (const MPI_Comm             native, const bool managed = false)
  : communicator(native, managed)
  {

  }
  explicit graph_communicator   (const communicator&        that, const graph& graph, const bool reorder = true)
  : communicator(MPI_COMM_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Graph_create, (that.native(), static_cast<std::int32_t>(graph.degrees.size()), graph.degrees.data(), graph.edges.data(), reorder, &native_))
  }
  graph_communicator            (const graph_communicator&  that)          = default;
  graph_communicator            (      graph_communicator&& temp) noexcept = default;
 ~graph_communicator            ()                                override = default;
  graph_communicator& operator= (const graph_communicator&  that)          = default;
  graph_communicator& operator= (      graph_communicator&& temp) noexcept = default;
  
  bool                operator==(const graph_communicator&  that) const
  {
    return compare(that) == comparison::identical;
  }

  [[nodiscard]]
  std::array<std::int32_t, 2> counts        () const
  {
    std::array<std::int32_t, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Graphdims_get, (native_, &result[0], &result[1]))
    return result;
  }
  [[nodiscard]]               
  graph                       graph         () const
  {
    const auto count = counts();

    mpi::graph result { std::vector<std::int32_t>(count[0]), std::vector<std::int32_t>(count[1]) };
    MPI_CHECK_ERROR_CODE(MPI_Graph_get, (native_, count[0], count[1], result.degrees.data(), result.edges.data()))
    return result;
  }
  [[nodiscard]]               
  std::int32_t                map           (const mpi::graph& graph) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Graph_map, (native_, static_cast<std::int32_t>(graph.degrees.size()), graph.degrees.data(), graph.edges.data(), &result))
    // MPI_CHECK_UNDEFINED (MPI_Graph_map, result) // MPI_UNDEFINED should not cause an exception in this case.
    return result;
  }

  [[nodiscard]]               
  std::int32_t                neighbor_count(const std::int32_t rank) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Graph_neighbors_count, (native_, rank, &result))
    return result;
  }
  [[nodiscard]]               
  std::vector<std::int32_t>   neighbors     (const std::int32_t rank) const
  {
    std::vector<std::int32_t> result(neighbor_count(rank));
    MPI_CHECK_ERROR_CODE(MPI_Graph_neighbors, (native_, rank, static_cast<std::int32_t>(result.size()), result.data()))
    return result;
  }
};
}