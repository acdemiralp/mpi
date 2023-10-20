#include "internal/doctest.h"

#define MPI_USE_EXCEPTIONS

#include <mpi/all.hpp>

TEST_CASE("C Interface")
{
  std::int32_t argc;
  char**       argv;

  MPI_Init(&argc, &argv);

  std::int32_t communicator_rank, communicator_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &communicator_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &communicator_size);

  {
    std::int32_t value(0);
    if (communicator_rank == 0)
      value = 42;

    if (communicator_rank == 0)
      MPI_Send(&value, 1, MPI_INT32_T, 1, 0, MPI_COMM_WORLD);
    if (communicator_rank == 1)
      MPI_Recv(&value, 1, MPI_INT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  {
    std::int32_t value = 0;
    if (communicator_rank == 0)
      value = 42;

    MPI_Bcast(&value, 1, MPI_INT32_T, 0, MPI_COMM_WORLD);
  }

  {
    struct particle
    {
      std::uint64_t id;
      float         position[3];
    };

    particle value = {};
    if (communicator_rank == 0)
      value = {42ull, {1.0f, 2.0f, 3.0f}};

    MPI_Datatype position_data_type;
    MPI_Type_contiguous(3, MPI_FLOAT, &position_data_type);
    MPI_Type_commit    (&position_data_type);

    MPI_Datatype particle_data_type;
    std::array              block_lengths {1, 1};
    std::array<MPI_Aint, 2> displacements {0, sizeof (std::uint64_t)};
    std::array              data_types    {MPI_UINT64_T, position_data_type};
    MPI_Type_struct    (2, block_lengths.data(), displacements.data(), data_types.data(), &particle_data_type);
    MPI_Type_commit    (&particle_data_type);

    MPI_Bcast          (&value, 1, particle_data_type, 0, MPI_COMM_WORLD);

    MPI_Type_free      (&position_data_type);
    MPI_Type_free      (&particle_data_type);
  }

  {
    std::vector<float> local_values;
    if (communicator_rank == 0)
      local_values.resize(10, 1.0f);
    if (communicator_rank == 1)
      local_values.resize(20, 2.0f);

    const auto                local_size    (static_cast<std::int32_t>(local_values.size()));
    std::vector<std::int32_t> gathered_sizes(communicator_size);
    MPI_Gather(&local_size, 1, MPI_INT32_T, gathered_sizes.data(), communicator_size, MPI_INT32_T, 0, MPI_COMM_WORLD);
    
    std::vector<float> gathered_values;
    if (communicator_rank == 0)
      gathered_values.resize(std::reduce(gathered_sizes.begin(), gathered_sizes.end()));
      
    std::vector<std::int32_t> displacements(gathered_sizes.size());
    std::exclusive_scan(gathered_sizes.begin(), gathered_sizes.end(), displacements.begin(), 0);

    MPI_Gatherv(local_values.data(), static_cast<std::int32_t>(local_values.size()), MPI_FLOAT, gathered_values.data(), gathered_sizes.data(), displacements.data(), MPI_FLOAT, 0, MPI_COMM_WORLD);
  }

  {
    std::vector<std::int32_t> values(communicator_size);
    if (communicator_rank == 0)
      std::iota(values.begin(), values.end(), 0);

    std::int32_t local_value(0);

    MPI_Request request;
    MPI_Iscatter    (values.data(), 1, MPI_INT32_T, &local_value, 1, MPI_INT32_T, 0, MPI_COMM_WORLD, &request);
    MPI_Wait        (&request, MPI_STATUS_IGNORE);
    MPI_Request_free(&request);
    local_value *= communicator_rank;
    MPI_Igather     (&local_value, 1, MPI_INT32_T, values.data(), 1, MPI_INT32_T, 0, MPI_COMM_WORLD, &request);
    MPI_Wait        (&request, MPI_STATUS_IGNORE);
    MPI_Request_free(&request);
  }

  {
    MPI_Info information;
    MPI_Info_create(&information);

    MPI_File file;
    MPI_File_open     (MPI_COMM_WORLD, "test.txt", MPI_MODE_CREATE | MPI_MODE_RDWR, information, &file);
    MPI_File_write_all(file, &communicator_rank, 1, MPI_INT32_T, MPI_STATUS_IGNORE);
    MPI_File_close    (&file);

    MPI_Info_free(&information);
  }

  {
    // TODO: One-sided communication.
  }

  {
    // TODO: Tool interface.
  }

  MPI_Finalize ();
}