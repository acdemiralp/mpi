#pragma once

#include <cstdint>
#include <functional>

#include <mpi/core/mpi.hpp>

namespace mpi
{
class op
{
public:
  op           (bool commutative)
  {
    MPI_Op_create([ ] (void* in, void* inout, int* length, MPI_Datatype* type)
    {

    }, static_cast<std::int32_t>(commutative), &native_);
  }
  op           (const op&  that) = delete;
  op           (      op&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_OP_NULL;
  }
  virtual ~op  ()
  {
    if (managed_ && native_ != MPI_OP_NULL)
      MPI_Op_free(&native_);
  }
  op& operator=(const op&  that) = delete;
  op& operator=(      op&& temp) noexcept
  {
    if (this != &temp)
    {
      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_OP_NULL;
    }
    return *this;
  }

  bool commutative() const
  {
    std::int32_t result;
    MPI_Op_commutative(native_, &result);
    return static_cast<bool>(result);
  }

protected:
  bool   managed_ = false;
  MPI_Op native_  = MPI_OP_NULL;
};
}