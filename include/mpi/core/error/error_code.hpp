#pragma once

#include <cstdint>

#include <mpi/core/error/error_class.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class error_code : public error_class // The error classes are a subset of the error codes.
{
public:
  explicit error_code    (const error_class& error_class)
  {
    MPI_Add_error_code(error_class.native(), &native_);
  }
  explicit error_code    (const std::int32_t native) : mpi::error_class(native)
  {

  }
  error_code             (const error_code&  that) = default;
  error_code             (      error_code&& temp) = default;
  ~error_code            () override               = default;
  error_code& operator=  (const error_code&  that) = default;
  error_code& operator=  (      error_code&& temp) = default;

  bool        operator== (const error_code&  that) const
  {
    return native_ == that.native_;
  }
  bool        operator!= (const error_code&  that) const
  {
    return native_ != that.native_;
  }

  [[nodiscard]]
  error_class error_class() const
  {
    std::int32_t result;
    MPI_Error_class(native_, &result);
    return mpi::error_class(result);
  }
};
}