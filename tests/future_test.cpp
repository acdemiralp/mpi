#include "internal/doctest.h"

#include <mpi/all.hpp>

TEST_CASE("Future Test")
{
  mpi::environment environment  ;
  const auto&      communicator = mpi::world_communicator;

  std::int32_t data = 0;
  if (communicator.rank() == 0)
    data = 1;

  auto final_status = mpi::future(communicator.immediate_broadcast(data, 0))
    .then([&] (mpi::future future)
    {
      auto status = future.get();
      if (communicator.rank() == 1)
        data++;
      return communicator.immediate_broadcast(data, 1);
    })
    .then([&] (mpi::future future)
    {
      auto status = future.get();
      if (communicator.rank() == 0)
        data++;
      return communicator.immediate_broadcast(data, 0);
    })
    .get();
  REQUIRE(data == 3);
  
  auto final_status_2 = mpi::make_ready_future()
    .then([&] (mpi::future future)
    {
      auto status = future.get();
      if (communicator.rank() == 0)
        data = 1;
      return communicator.immediate_broadcast(data, 0);
    })
    .then([&] (mpi::future future)
    {
      auto status = future.get();
      if (communicator.rank() == 1)
        data++;
      return communicator.immediate_broadcast(data, 1);
    })
    .then([&] (mpi::future future)
    {
      auto status = future.get();
      if (communicator.rank() == 0)
        data++;
      return communicator.immediate_broadcast(data, 0);
    })
    .get();
  REQUIRE(data == 3);
}