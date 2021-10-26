#include "internal/doctest.h"

#include <iostream>

#include <mpi/all.hpp>

TEST_CASE("MPI Test")
{
  mpi::environment environment  ;
  const auto&      communicator = mpi::world_communicator;

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
    std::cout << data_container[0] << " "
              << data_container[1] << " "
              << data_container[2] << std::endl;
  }

  // User type test.
  struct user_type
  {
    std::int32_t         id      ;
    std::array<float, 3> position;
  };

  user_type user_object;
  if (communicator.rank() == 0)
  {
    user_object = {42, {0.0f, 1.0f, 2.0f}};
    communicator.send   (user_object, 1);
  }
  if (communicator.rank() == 1)
  {
    communicator.receive(user_object, 0);
    REQUIRE(user_object.id          == 42  );
    REQUIRE(user_object.position[0] == 0.0f);
    REQUIRE(user_object.position[1] == 1.0f);
    REQUIRE(user_object.position[2] == 2.0f);
    std::cout << user_object.id          << " "
              << user_object.position[0] << " "
              << user_object.position[1] << " "
              << user_object.position[2] << std::endl;
  }
  
  // Container of user type test.
  std::vector<user_type> user_object_container(2);
  if (communicator.rank() == 0)
  {
    user_object_container[0] = {42, {0.0f, 1.0f, 2.0f}};
    user_object_container[1] = {84, {3.0f, 4.0f, 5.0f}};
    communicator.send   (user_object_container, 1);
  }
  if (communicator.rank() == 1)
  {
    communicator.receive(user_object_container, 0);
    REQUIRE(user_object_container[0].id          == 42  );
    REQUIRE(user_object_container[0].position[0] == 0.0f);
    REQUIRE(user_object_container[0].position[1] == 1.0f);
    REQUIRE(user_object_container[0].position[2] == 2.0f);
    REQUIRE(user_object_container[1].id          == 84  );
    REQUIRE(user_object_container[1].position[0] == 3.0f);
    REQUIRE(user_object_container[1].position[1] == 4.0f);
    REQUIRE(user_object_container[1].position[2] == 5.0f);
    std::cout << user_object_container[0].id          << " "
              << user_object_container[0].position[0] << " "
              << user_object_container[0].position[1] << " "
              << user_object_container[0].position[2] << std::endl;
    std::cout << user_object_container[1].id          << " "
              << user_object_container[1].position[0] << " "
              << user_object_container[1].position[1] << " "
              << user_object_container[1].position[2] << std::endl;
  }
}