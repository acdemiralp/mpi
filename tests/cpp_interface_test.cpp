#include "internal/doctest.h"

#define MPI_USE_EXCEPTIONS

#include <mpi/all.hpp>

TEST_CASE("C++ Interface")
{
  std::int32_t argc;
  char**       argv;

  mpi::environment environment(&argc, &argv);


  const auto communicator_rank = mpi::world_communicator.rank();
  const auto communicator_size = mpi::world_communicator.size();

  {
    std::int32_t value(0);
    if (communicator_rank == 0)
      value = 42;

    if (communicator_rank == 0)
      mpi::world_communicator.send   (value, 1, 0);
    if (communicator_rank == 1)
      mpi::world_communicator.receive(value, 0, 0);
  }

  {
    std::int32_t value(0);
    if (communicator_rank == 0)
      value = 42;

    mpi::world_communicator.broadcast(value, 0);
  }

  {
    struct particle
    {
      std::uint64_t        id;
      std::array<float, 3> position;
    };

    particle value = {};
    if (communicator_rank == 0)
      value = {42ull, {1.0f, 2.0f, 3.0f}};












    mpi::world_communicator.broadcast(value, 0);

    // Types are freed at std::exit.

  }

  {
    std::vector<float> local_values;
    if (communicator_rank == 0)
      local_values.resize(10, 1.0f);
    if (communicator_rank == 1)
      local_values.resize(20, 2.0f);





    std::vector<float> gathered_values;






    mpi::world_communicator.gather_varying(local_values, gathered_values, 0, true);
  }

  {
    std::vector<std::int32_t> values(communicator_size);
    if (communicator_rank == 0)
      std::iota(values.begin(), values.end(), 0);

    std::int32_t local_value(0);

    mpi::future(mpi::world_communicator.immediate_scatter(values, local_value, 0))
    .then ([&] (auto future)
    {
      future.get();
      local_value *= communicator_rank;
      return mpi::world_communicator.immediate_gather(local_value, values, 0);
    })
    .get();
  }

  {
    



    const mpi::io::file file(mpi::world_communicator, "test.txt", mpi::io::access_mode::create | mpi::io::access_mode::read_write);
    file.write_all(communicator_rank);
    // File is freed at scope exit.


  }

  {
    // TODO: One-sided communication.
  }

  {
    // TODO: Tool interface.
  }

  // mpi::environment takes care of finalization at scope exit.
}