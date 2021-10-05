#pragma once

#include <cstdint>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class status
{
public:
  explicit status  (const MPI_Status& native) : native_(native)
  {
    
  }
  status           ()                    = default;
  status           (const status&  that) = default;
  status           (      status&& temp) = default;
  virtual ~status  ()                    = default;
  status& operator=(const status&  that) = default;
  status& operator=(      status&& temp) = default;

  void              set_cancelled           (const bool cancelled)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_cancelled, (&native_, static_cast<std::int32_t>(cancelled)))
  }
  [[nodiscard]]
  bool              cancelled               () const
  {
    auto result(0);
    MPI_CHECK_ERROR_CODE(MPI_Test_cancelled, (&native_, &result))
    return static_cast<bool>(result);
  }

  [[nodiscard]]
  std::int32_t      count_low               () const
  {
    return native_.count_lo;
  }
  [[nodiscard]]
  std::int32_t      count_high_and_cancelled() const
  {
    return native_.count_hi_and_cancelled;
  }
  [[nodiscard]]
  std::int32_t      source                  () const
  {
    return native_.MPI_SOURCE;
  }
  [[nodiscard]]
  std::int32_t      tag                     () const
  {
    return native_.MPI_SOURCE;
  }
  [[nodiscard]]
  error_code        error                   () const
  {
    return error_code(native_.MPI_ERROR);
  }

  [[nodiscard]]
  const MPI_Status& native                  () const
  {
    return native_;
  }
  
protected:
  MPI_Status native_ {0, 0, 0, 0, 0};
};
}