#pragma once

#include <cstdint>
#include <vector>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/structs/neighbor_information.hpp>
#include <mpi/core/structs/distributed_graph.hpp>
#include <mpi/core/structs/neighbor_counts.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class distributed_graph_communicator : public communicator
{
public:
  explicit distributed_graph_communicator   (const MPI_Comm native, const bool managed = false)
  : communicator(native, managed)
  {

  }
  explicit distributed_graph_communicator   (const communicator& that, const distributed_graph&    graph, const mpi::information& info = mpi::information(), const bool reorder = true)
  : communicator(MPI_COMM_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_create, (
      that.native(), 
      static_cast<std::int32_t>(graph.sources.size()), 
      graph.sources     .data(), 
      graph.degrees     .data(), 
      graph.destinations.data(), 
      graph.weights ? graph.weights->data() : MPI_UNWEIGHTED,
      info .native(),
      reorder, 
      &native_))
  }
  explicit distributed_graph_communicator   (const communicator& that, const neighbor_information& graph, const mpi::information& info = mpi::information(), const bool reorder = true)
  : communicator(MPI_COMM_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_create_adjacent, (
      that.native(),
      static_cast<std::int32_t>(graph.sources.size()), 
      graph.sources     .data(), 
      graph.source_weights      ? graph.source_weights     ->data() : MPI_UNWEIGHTED,
      static_cast<std::int32_t>(graph.destinations.size()),
      graph.destinations.data(), 
      graph.destination_weights ? graph.destination_weights->data() : MPI_UNWEIGHTED,
      info .native(),
      reorder, 
      &native_))
  }
  distributed_graph_communicator            (const distributed_graph_communicator&  that)          = default;
  distributed_graph_communicator            (      distributed_graph_communicator&& temp) noexcept = default;
 ~distributed_graph_communicator            ()                                            override = default;
  distributed_graph_communicator& operator= (const distributed_graph_communicator&  that)          = default;
  distributed_graph_communicator& operator= (      distributed_graph_communicator&& temp) noexcept = default;
  
  bool                            operator==(const distributed_graph_communicator&  that) const
  {
    return compare(that) == comparison::identical;
  }

  [[nodiscard]]
  neighbor_counts      neighbor_counts() const
  {
    mpi::neighbor_counts result {};
    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_neighbors_count, (native_, &result.source_count, &result.destination_count, reinterpret_cast<std::int32_t*>(&result.weighted)))
    return result;
  }
  [[nodiscard]]
  neighbor_information neighbors      () const
  {
    const auto info = neighbor_counts();

    neighbor_information result
    {
      std::vector<std::int32_t>(info.source_count     ),
      info.weighted ? std::vector<std::int32_t>(info.source_count     ) : std::vector<std::int32_t>(),
      std::vector<std::int32_t>(info.destination_count),
      info.weighted ? std::vector<std::int32_t>(info.destination_count) : std::vector<std::int32_t>()
    };

    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_neighbors, (
      native_, 
      info.source_count, 
      result.sources     .data(), 
      info.weighted ? result.source_weights     ->data() : MPI_UNWEIGHTED, 
      info.destination_count, 
      result.destinations.data(), 
      info.weighted ? result.destination_weights->data() : MPI_UNWEIGHTED))

    return result;
  }
};
}