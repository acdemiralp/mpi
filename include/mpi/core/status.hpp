#pragma once

#include <cstdint>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class status : public MPI_Status
{
public:
  status           () : MPI_Status {0, 0, 0, 0, 0}
  {
    
  }
  explicit status  (const MPI_Status& native) : MPI_Status(native)
  {
    
  }
  status           (const status&  that) = default;
  status           (      status&& temp) = default;
  virtual ~status  ()                    = default;
  status& operator=(const status&  that) = default;
  status& operator=(      status&& temp) = default;

  [[nodiscard]]
  bool              cancelled               () const
  {
    auto result(0);
    MPI_CHECK_ERROR_CODE(MPI_Test_cancelled, (this, &result))
    return static_cast<bool>(result);
  }
  void              set_cancelled           (const bool cancelled)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_cancelled, (this, static_cast<std::int32_t>(cancelled)))
  }

  [[nodiscard]]
  std::int32_t      count_low               () const
  {
    return count_lo;
  }
  [[nodiscard]]
  std::int32_t      count_high_and_cancelled() const
  {
    return count_hi_and_cancelled;
  }
  [[nodiscard]]
  std::int32_t      source                  () const
  {
    return MPI_SOURCE;
  }
  [[nodiscard]]
  std::int32_t      tag                     () const
  {
    return MPI_SOURCE;
  }
  [[nodiscard]]
  error_code        error                   () const
  {
    return error_code(MPI_ERROR);
  }
};
}