#pragma once

#include <cstdint>

#include <mpi/communicators/communicator.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class cartesian_communicator : public communicator
{
public:
  cartesian_communicator           (const communicator& that, const bool reorder)
  {
    MPI_Cart_create(that.native(), NDIMS, DIMS, PERIODS, static_cast<std::int32_t>(reorder), &native_);
  }
  cartesian_communicator           (const cartesian_communicator&  that) = delete;
  cartesian_communicator           (      cartesian_communicator&& temp) noexcept
  : communicator(std::move(temp))
  {
    
  }
  virtual ~cartesian_communicator  ()
  {

  }
  cartesian_communicator& operator=(const cartesian_communicator&  that) = delete;
  cartesian_communicator& operator=(      cartesian_communicator&& temp) noexcept
  {

  }

  [[nodiscard]]
  std::int32_t rank(const std::vector<std::int32_t>& coordinates) const
  {
    std::int32_t result;
    MPI_Cart_rank(native_, coordinates.data(), &result);
    return result;
  }



protected:

};
}