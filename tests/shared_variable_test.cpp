#include "internal/doctest.h"

#define MPI_USE_EXCEPTIONS

#include <mpi/all.hpp>

TEST_CASE("Shared Variable Test")
{
  mpi::environment environment  ;
  const auto&      communicator = mpi::world_communicator;

  {
    mpi::manual_shared_variable<std::int32_t> variable(communicator);

    if (communicator.rank() == 0)
      variable = 42;

    variable.synchronize(); // :(

    if (communicator.rank() != 0)
      REQUIRE(variable == 42);
  }
  
  {
    mpi::manual_shared_variable<std::array<std::int32_t, 3>> variable(communicator);
    if (communicator.rank() == 0)
      variable = {1, 2, 3};
  
    variable.synchronize(); // :(
  
    if (communicator.rank() != 0)
    {
      auto value = variable.get();
      REQUIRE(value[0] == 1);
      REQUIRE(value[1] == 2);
      REQUIRE(value[2] == 3);
    }
  }
  
  {
    mpi::shared_variable<std::int32_t> variable(communicator);
    variable.set_if_rank(42, 0);
    if (communicator.rank() != 0)
      REQUIRE(variable == 42);
  }

  {
    mpi::shared_variable<std::array<std::int32_t, 3>> variable(communicator);
    variable.set_if_rank({1, 2, 3}, 0);
    if (communicator.rank() != 0)
    {
      auto value = variable.get();
      REQUIRE(value[0] == 1);
      REQUIRE(value[1] == 2);
      REQUIRE(value[2] == 3);
    }
  }
}