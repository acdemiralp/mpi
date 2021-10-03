#pragma once

#include <cstdint>
#include <string>

#include <mpi/core/mpi.hpp>

namespace mpi
{
class error_class
{
public:
  error_class           ()
  {
    MPI_Add_error_class(&native_);
  }
  explicit error_class  (const std::int32_t native) : native_(native)
  {
    
  }
  error_class           (const error_class&  that) = default;
  error_class           (      error_class&& temp) = default;
  virtual ~error_class  ()                         = default;
  error_class& operator=(const error_class&  that) = default;
  error_class& operator=(      error_class&& temp) = default;

  bool         set_string(const std::string& string) const
  {
    return MPI_Add_error_string(native_, string.c_str()) == MPI_SUCCESS;
  }
  [[nodiscard]]
  std::string  string     () const
  {
    std::string  result(MPI_MAX_ERROR_STRING, ' ');
    std::int32_t length(0);
    MPI_Error_string(native_, &result[0], &length);
    result.resize(length);
    return result;
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