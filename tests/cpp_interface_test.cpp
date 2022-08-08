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
    // TODO: Asynchronous calls, futures.
  }

  {
    // TODO: IO.
  }

  // mpi::environment takes care of finalization at scope exit.
}