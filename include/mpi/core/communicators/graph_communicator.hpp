#pragma once

#include <cstdint>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class graph_communicator : public communicator
{
public:
  explicit graph_communicator   (const MPI_Comm native)
  : communicator(native)
  {

  }
  explicit graph_communicator   (const communicator&        that, const std::vector<std::int32_t>& degrees, const std::vector<std::int32_t>& edges, const bool reorder = true)
  : communicator()
  {
    // TODO: Make arguments more sensible.

    managed_ = true;

    MPI_CHECK_ERROR_CODE(MPI_Graph_create, (that.native(), static_cast<std::int32_t>(degrees.size()), degrees.data(), edges.data(), static_cast<std::int32_t>(reorder), &native_))
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

  // Get, map, neighbors, neighbors_count, dims_get.
};
}