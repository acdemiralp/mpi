#pragma once

#include <cstdint>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/exception.hpp>
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
  distributed_graph_communicator            (const distributed_graph_communicator&  that)          = default;
  distributed_graph_communicator            (      distributed_graph_communicator&& temp) noexcept = default;
 ~distributed_graph_communicator            ()                                            override = default;
  distributed_graph_communicator& operator= (const distributed_graph_communicator&  that)          = default;
  distributed_graph_communicator& operator= (      distributed_graph_communicator&& temp) noexcept = default;
  
  bool                            operator==(const distributed_graph_communicator&  that) const
  {
    return compare(that) == comparison::identical;
  }

  // Create, create_adjacent, neighbors, neighbors_count.
};
}