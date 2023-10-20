#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
#ifdef MPI_GEQ_4_0
enum class callback_safety : std::int32_t
{
  none              = MPI_T_CB_REQUIRE_NONE             ,
  mpi_restricted    = MPI_T_CB_REQUIRE_MPI_RESTRICTED   ,
  thread_safe       = MPI_T_CB_REQUIRE_THREAD_SAFE      ,
  async_signal_safe = MPI_T_CB_REQUIRE_ASYNC_SIGNAL_SAFE
};
#endif
}