#include "internal/doctest.h"

#include <iostream>

#include <mpi/all.hpp>

TEST_CASE("MPI Test")
{
  mpi::environment         environment  ;
  const mpi::communicator& communicator = mpi::world_communicator;

  // Single compliant test.
  std::int32_t data = 0;
  if (communicator.rank() == 0)
  {
    data = 42;
    communicator.send   (data, 1);
  }
  if (communicator.rank() == 1)
  {
    communicator.receive(data, 0);
    REQUIRE(data == 42);
    std::cout << data << std::endl;
  }

  // Container of compliant test.
  std::vector<std::int32_t> data_container(3);
  if (communicator.rank() == 0)
  {
    data_container = {1, 2, 3};
    communicator.send   (data_container, 1);
  }
  if (communicator.rank() == 1)
  {
    communicator.receive(data_container, 0);
    REQUIRE(data_container[0] == 1);
    REQUIRE(data_container[1] == 2);
    REQUIRE(data_container[2] == 3);
    std::cout << data_container[0] << " " << data_container[1] << " " << data_container[2] << std::endl;
  }
}