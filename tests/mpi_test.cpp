#include "internal/doctest.h"

#include <array>
#include <cstdint>
#include <string>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/type/data_type_reflection.hpp>
#include <mpi/core/environment.hpp>

struct user_type
{
  std::int32_t              x;
  std::vector<std::int32_t> y;
  std::string               z;
};

TEST_CASE("MPI Test")
{
  mpi::environment environment;

  auto user_data_type = mpi::type_traits<user_type>::get_data_type(); // TODO: vector and string still resolve to the aggregate rather then specializations.
}