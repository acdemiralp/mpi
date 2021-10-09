#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class key_value
{
public:
  explicit key_value  (const std::int32_t native)
  : native_(native)
  {
    
  }
  key_value           (const key_value&  that) = delete ;
  key_value           (      key_value&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_KEYVAL_INVALID;
  }
  virtual ~key_value  ()                       = default;
  key_value& operator=(const key_value&  that) = delete ;
  key_value& operator=(      key_value&& temp) = default;

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
  key_value() = default; // Default constructor is only available to sub classes who control the member variables explicitly.

  bool         managed_ = false;
  std::int32_t native_  = MPI_KEYVAL_INVALID;
};

class communicator_key_value : public key_value
{
public:
  using copy_function_type   = std::int32_t (*) (MPI_Comm, std::int32_t, void*, void*, void*, std::int32_t*);
  using delete_function_type = std::int32_t (*) (MPI_Comm, std::int32_t, void*, void*);

  explicit communicator_key_value  (
    const copy_function_type   copy_function   = MPI_COMM_DUP_FN        ,
    const delete_function_type delete_function = MPI_COMM_NULL_DELETE_FN,
    void*                      extra_state     = nullptr                )
  {
    managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_keyval, (copy_function, delete_function, &native_, extra_state))
  }
  explicit communicator_key_value  (const std::int32_t native)
  : key_value(native)
  {
    
  }
  communicator_key_value           (const communicator_key_value&  that)          = delete ;
  communicator_key_value           (      communicator_key_value&& temp) noexcept = default;
 ~communicator_key_value           ()                                    override
  {
    if (managed_ && native_ != MPI_KEYVAL_INVALID)
      MPI_CHECK_ERROR_CODE(MPI_Comm_free_keyval, (&native_))
  }
  communicator_key_value& operator=(const communicator_key_value&  that)          = delete ;
  communicator_key_value& operator=(      communicator_key_value&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_KEYVAL_INVALID)
        MPI_CHECK_ERROR_CODE(MPI_Comm_free_keyval, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_KEYVAL_INVALID;
    }
    return *this;
  }
};
  
class data_type_key_value : public key_value
{
public:
  using copy_function_type   = std::int32_t (*) (MPI_Datatype, std::int32_t, void*, void*, void*, std::int32_t*);
  using delete_function_type = std::int32_t (*) (MPI_Datatype, std::int32_t, void*, void*);

  explicit data_type_key_value  (
    const copy_function_type   copy_function   = MPI_TYPE_DUP_FN        ,
    const delete_function_type delete_function = MPI_TYPE_NULL_DELETE_FN,
    void*                      extra_state     = nullptr                )
  {
    managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Type_create_keyval, (copy_function, delete_function, &native_, extra_state))
  }
  explicit data_type_key_value  (const std::int32_t native)
  : key_value(native)
  {
    
  }
  data_type_key_value           (const data_type_key_value&  that)          = delete ;
  data_type_key_value           (      data_type_key_value&& temp) noexcept = default;
 ~data_type_key_value           ()                                 override
  {
    if (managed_ && native_ != MPI_KEYVAL_INVALID)
      MPI_CHECK_ERROR_CODE(MPI_Type_free_keyval, (&native_))
  }
  data_type_key_value& operator=(const data_type_key_value&  that)          = delete ;
  data_type_key_value& operator=(      data_type_key_value&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_KEYVAL_INVALID)
        MPI_CHECK_ERROR_CODE(MPI_Type_free_keyval, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_KEYVAL_INVALID;
    }
    return *this;
  }
};
  
class window_key_value : public key_value
{
public:
  using copy_function_type   = std::int32_t (*) (MPI_Win, std::int32_t, void*, void*, void*, std::int32_t*);
  using delete_function_type = std::int32_t (*) (MPI_Win, std::int32_t, void*, void*);

  explicit window_key_value  (
    const copy_function_type   copy_function   = MPI_WIN_DUP_FN        ,
    const delete_function_type delete_function = MPI_WIN_NULL_DELETE_FN,
    void*                      extra_state     = nullptr               )
  {
    managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Win_create_keyval, (copy_function, delete_function, &native_, extra_state))
  }
  explicit window_key_value  (const std::int32_t native)
  : key_value(native)
  {
    
  }
  window_key_value           (const window_key_value&  that)          = delete ;
  window_key_value           (      window_key_value&& temp) noexcept = default;
 ~window_key_value           ()                              override
  {
    if (managed_ && native_ != MPI_KEYVAL_INVALID)
      MPI_CHECK_ERROR_CODE(MPI_Win_free_keyval, (&native_))
  }
  window_key_value& operator=(const window_key_value&  that)          = delete ;
  window_key_value& operator=(      window_key_value&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_KEYVAL_INVALID)
        MPI_CHECK_ERROR_CODE(MPI_Win_free_keyval, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_KEYVAL_INVALID;
    }
    return *this;
  }
};
}