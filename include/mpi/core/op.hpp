#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class op
{
public:
  using function_type = void (*) (void*, void*, std::int32_t*, MPI_Datatype*);

  explicit op  (const function_type function, const bool commutative)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Op_create, (function, static_cast<std::int32_t>(commutative), &native_))
  }
  explicit op  (const MPI_Op native)
  : native_(native)
  {
    
  }
  op           (const op&    that) = delete;
  op           (      op&&   temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_OP_NULL;
  }
  virtual ~op  ()
  {
    if (managed_ && native_ != MPI_OP_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Op_free, (&native_))
  }   
  op& operator=(const op&    that) = delete;
  op& operator=(      op&&   temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_OP_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Op_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_OP_NULL;
    }
    return *this;
  }

  [[nodiscard]]
  bool   commutative() const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Op_commutative, (native_, &result))
    return static_cast<bool>(result);
  }

  [[nodiscard]]
  bool   managed    () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Op native     () const
  {
    return native_;
  }

protected:
  bool   managed_ = false;
  MPI_Op native_  = MPI_OP_NULL;
};
}