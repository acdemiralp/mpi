#include "internal/doctest.h"

#include <iostream>

#include <mpi/all.hpp>

TEST_CASE("MPI Test")
{
  mpi::environment         environment  ;
  const mpi::communicator& communicator = mpi::world_communicator;

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
}