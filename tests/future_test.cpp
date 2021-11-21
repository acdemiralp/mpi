#include "internal/doctest.h"

#define MPI_USE_EXCEPTIONS

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

  std::int32_t data_1 = 0;
  std::int32_t data_2 = 0;
  if (communicator.rank() == 0)
    data_1 = 1;
  if (communicator.rank() == 1)
    data_2 = 1;

  std::vector<mpi::request> requests;
  requests.push_back(communicator.immediate_broadcast(data_1, 0));
  requests.push_back(communicator.immediate_broadcast(data_2, 1));
  wait_all(requests);

  REQUIRE(data_1 == 1);
  REQUIRE(data_2 == 1);

  if (communicator.rank() == 0)
    data_1 = 2;
  if (communicator.rank() == 1)
    data_2 = 2;

  requests.clear();
  requests.push_back(communicator.immediate_broadcast(data_1, 0));
  requests.push_back(communicator.immediate_broadcast(data_2, 1));
  when_all(requests).get();

  REQUIRE(data_1 == 2);
  REQUIRE(data_2 == 2);
}