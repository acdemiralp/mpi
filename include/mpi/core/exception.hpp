#pragma once

#include <stdexcept>
#include <string>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/error/standard_error_classes.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class exception : public std::runtime_error
{
public:
  explicit exception  (const std::string& function_name, const error_code& code)
  : std::runtime_error(std::string(function_name + " failed with code " + std::to_string(code.native()) + ": " + code.string()))
  , function_name_    (function_name)
  , code_             (code)
  {
    
  }
  exception           (const exception&  that) = default;
  exception           (      exception&& temp) = default;
 ~exception           () override              = default;
  exception& operator=(const exception&  that) = default;
  exception& operator=(      exception&& temp) = default;

  [[nodiscard]]
  const std::string& function_name() const
  {
    return function_name_;
  }
  [[nodiscard]]
  const error_code&  code         () const
  {
    return code_;
  }

protected:
  std::string function_name_;
  error_code  code_         ;
};

#ifdef MPI_USE_EXCEPTIONS
#define MPI_CHECK_ERROR_CODE(FUNC, ARGS)                           \
auto code = FUNC ARGS;                                             \
if (code != MPI_SUCCESS)                                           \
  throw mpi::exception(std::string(#FUNC), mpi::error_code(code));

#define MPI_CHECK_UNDEFINED(FUNC, VALUE)                           \
if ((VALUE) == MPI_UNDEFINED)                                      \
  throw mpi::exception(std::string(#FUNC), mpi::error::size); 
#else
#define MPI_CHECK_ERROR_CODE(FUNC, ARGS) FUNC ARGS;
#define MPI_CHECK_UNDEFINED(FUNC, VALUE) ; 
#endif
}