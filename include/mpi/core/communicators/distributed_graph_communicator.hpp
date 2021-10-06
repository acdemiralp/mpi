#pragma once

#include <cstdint>
#include <vector>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/structs/adjacent_distributed_graph.hpp>
#include <mpi/core/structs/distributed_graph.hpp>
#include <mpi/core/structs/distributed_graph_information.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class distributed_graph_communicator : public communicator
{
public:
  explicit distributed_graph_communicator   (const MPI_Comm native)
  : communicator(native)
  {

  }
  explicit distributed_graph_communicator   (const communicator& that, const distributed_graph&          graph, const mpi::information& info = mpi::information(), const bool reorder = true)
  : communicator()
  {
    managed_ = true;

    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_create, (
      that.native(), 
      static_cast<std::int32_t>(graph.sources.size()), 
      graph.sources     .data(), 
      graph.degrees     .data(), 
      graph.destinations.data(), 
      graph.weights ? graph.weights->data() : MPI_UNWEIGHTED,
      info .native(), 
      static_cast<std::int32_t>(reorder), 
      &native_))
  }
  explicit distributed_graph_communicator   (const communicator& that, const adjacent_distributed_graph& graph, const mpi::information& info = mpi::information(), const bool reorder = true)
  : communicator()
  {
    managed_ = true;

    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_create_adjacent, (
      that.native(),
      static_cast<std::int32_t>(graph.sources.size()), 
      graph.sources     .data(), 
      graph.source_weights      ? graph.source_weights     ->data() : MPI_UNWEIGHTED,
      static_cast<std::int32_t>(graph.destinations.size()),
      graph.destinations.data(), 
      graph.destination_weights ? graph.destination_weights->data() : MPI_UNWEIGHTED,
      info .native(), 
      static_cast<std::int32_t>(reorder), 
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
  distributed_graph_information neighbor_information() const
  {
    distributed_graph_information result {};
    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_neighbors_count, (native_, &result.in_degree, &result.out_degree, reinterpret_cast<std::int32_t*>(&result.weighted)))
    return result;
  }
  [[nodiscard]]
  adjacent_distributed_graph    neighbors           () const
  {
    const auto info = neighbor_information();

    adjacent_distributed_graph result
    {
      std::vector<std::int32_t>(info.in_degree ),
      info.weighted ? std::vector<std::int32_t>(info.in_degree ) : std::vector<std::int32_t>(),
      std::vector<std::int32_t>(info.out_degree),
      info.weighted ? std::vector<std::int32_t>(info.out_degree) : std::vector<std::int32_t>()
    };

    MPI_CHECK_ERROR_CODE(MPI_Dist_graph_neighbors, (
      native_, 
      info.in_degree, 
      result.sources     .data(), 
      info.weighted ? result.source_weights     ->data() : MPI_UNWEIGHTED, 
      info.out_degree, 
      result.destinations.data(), 
      info.weighted ? result.destination_weights->data() : MPI_UNWEIGHTED))

    return result;
  }
};
}