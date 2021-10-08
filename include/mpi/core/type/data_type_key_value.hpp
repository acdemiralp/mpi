#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class data_type_key_value
{
public:
  using copy_function   = std::int32_t (*) (MPI_Datatype, std::int32_t, void*, void*, void*, std::int32_t*);
  using delete_function = std::int32_t (*) (MPI_Datatype, std::int32_t, void*, void*);

  explicit data_type_key_value  (
    const copy_function   copy_function   = MPI_TYPE_DUP_FN        , 
    const delete_function delete_function = MPI_TYPE_NULL_DELETE_FN, 
    void*                 extra_state     = nullptr)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_keyval, (copy_function, delete_function, &native_, extra_state)) // Extra state is unused.
  }
  data_type_key_value           (const data_type_key_value&  that) = delete;
  data_type_key_value           (      data_type_key_value&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_KEYVAL_INVALID;
  }
  virtual ~data_type_key_value  ()
  {
    if (managed_ && native_ != MPI_KEYVAL_INVALID)
      MPI_CHECK_ERROR_CODE(MPI_Type_free_keyval, (&native_))
  }
  data_type_key_value& operator=(const data_type_key_value&  that) = delete;
  data_type_key_value& operator=(      data_type_key_value&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_KEYVAL_INVALID)
        MPI_CHECK_ERROR_CODE(MPI_Type_free_keyval, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_KEYVAL_INVALID;
    }
    return *this;
  }

  [[nodiscard]]
  bool         managed() const
  {
    return managed_;
  }
  [[nodiscard]]
  std::int32_t native () const
  {
    return native_;
  }

protected:
  bool         managed_ = false;
  std::int32_t native_  = MPI_KEYVAL_INVALID;
};
}