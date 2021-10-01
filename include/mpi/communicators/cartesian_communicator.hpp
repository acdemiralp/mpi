#pragma once

#include <cstdint>

#include <mpi/communicators/communicator.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class cartesian_communicator : public communicator
{
public:
  cartesian_communicator           (
    const communicator& old,
    const bool reorder)
  {
    MPI_Cart_create(old.native(), static_cast<std::int32_t>(reorder), &native_);
  }
  cartesian_communicator           (const cartesian_communicator&  that) = delete;
  cartesian_communicator           (      cartesian_communicator&& temp) noexcept
  {

  }
  virtual ~cartesian_communicator  ()
  {

  }
  cartesian_communicator& operator=(const cartesian_communicator&  that) = delete;
  cartesian_communicator& operator=(      cartesian_communicator&& temp) noexcept
  {

  }

protected:

};
}