#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
class status : public MPI_Status
{
public:
  status           ()                    = default;
  status           (const status&  that) = default;
  status           (      status&& temp) = default;
  status& operator=(const status&  that) = default;
  status& operator=(      status&& temp) = default;

  bool set_cancelled(const bool cancelled)
  {
    return MPI_Status_set_cancelled(this, static_cast<std::int32_t>(cancelled)) == MPI_SUCCESS;
  }

  [[nodiscard]]
  bool is_cancelled () const
  {
    auto result(0);
    MPI_Test_cancelled(this, &result);
    return static_cast<bool>(result);
  }

protected:

};
}