#pragma once

#include <cstdint>
#include <string>

#include <mpi/core/error/error_class.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class error_code
{
public:
  explicit error_code  (const error_class& error_class)
  {
    MPI_Add_error_code(error_class.native(), &native_);
  }
  explicit error_code  (const std::int32_t native) : native_(native)
  {
    
  }
  error_code           (const error_code&  that) = default;
  error_code           (      error_code&& temp) = default;
  virtual ~error_code  ()                        = default;
  error_code& operator=(const error_code&  that) = default;
  error_code& operator=(      error_code&& temp) = default;

  bool         set_string(const std::string& string) const
  {
    return MPI_Add_error_string(native_, string.c_str()) == MPI_SUCCESS;
  }

  [[nodiscard]]
  std::int32_t native    () const
  {
    return native_;
  }

protected:
  std::int32_t native_ = MPI_ERR_LASTCODE;
};

}