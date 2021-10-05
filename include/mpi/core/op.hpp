#pragma once

#include <cstdint>
#include <functional>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class op
{
public:
  using native_type = MPI_Op;

  explicit op  (const std::function<void(void*, void*, int*, MPI_Datatype*)>& function, const bool commutative)
  {
    // TODO: FIX. TARGET() DOES NOT WORK!
    MPI_CHECK_ERROR_CODE(MPI_Op_create, (*function.target<void(*)(void*, void*, int*, MPI_Datatype*)>(), static_cast<std::int32_t>(commutative), &native_))
  }
  explicit op  (const native_type native     )
  : native_(native)
  {
    
  }
  op           (const op&         that       ) = delete;
  op           (      op&&        temp       ) noexcept
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
  op& operator=(const op&         that       ) = delete;
  op& operator=(      op&&        temp       ) noexcept
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
  bool        commutative() const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Op_commutative, (native_, &result))
    return static_cast<bool>(result);
  }

  [[nodiscard]]
  bool        managed    () const
  {
    return managed_;
  }
  [[nodiscard]]
  native_type native     () const
  {
    return native_;
  }

protected:
  bool        managed_ = false;
  native_type native_  = MPI_OP_NULL;
};
}