#include "internal/doctest.h"

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/type/data_type_reflection.hpp>
#include <mpi/core/environment.hpp>

struct user_type
{
  std::int32_t                x;
  std::array<std::int32_t, 3> y;
  std::string                 z;
};

TEST_CASE("MPI Test")
{
  mpi::environment environment;

  //static_assert(mpi::is_compliant_aggregate<user_type>::value, "Not compliant.");

  static_assert(mpi::is_compliant<float>               ::value, "Is not compliant.");
  //static_assert(mpi::is_compliant<std::array<float, 4>>::value, "Is not compliant.");
}