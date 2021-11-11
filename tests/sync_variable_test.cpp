#include "internal/doctest.h"

#define MPI_USE_EXCEPTIONS

#include <mpi/all.hpp>

TEST_CASE("Sync Variable Test")
{
  mpi::environment environment  ;
  const auto&      communicator = mpi::world_communicator;

  for (auto i = 0; i < 1000; ++i)
  {
    mpi::sync_variable<std::int32_t> variable(communicator);
    if (communicator.rank() == 0)
      variable.put(i);

    variable.synchronize(); // Why is this necessary :(

    if (communicator.rank() != 0)
      REQUIRE(variable.get() == i);
  }

  for (auto i = 0; i < 1000; ++i)
  {
    mpi::sync_variable<std::array<std::int32_t, 3>> variable(communicator);
    if (communicator.rank() == 0)
      variable.put({i, i + 1, i + 2});

    variable.synchronize(); // Why is this necessary :(

    if (communicator.rank() != 0)
    {
      auto value = variable.get();
      REQUIRE(value[0] == i    );
      REQUIRE(value[1] == i + 1);
      REQUIRE(value[2] == i + 2);
    }
  }
}