#include "internal/doctest.h"

#include <array>
#include <cstdint>
#include <string>

#include <mpi/core/type/type_traits.hpp>
#include <mpi/core/environment.hpp>

struct aggregate
{
  std::int32_t                x;
  std::array<std::int32_t, 3> y;
};
struct non_aggregate
{
  std::int32_t                x;
  std::array<std::int32_t, 3> y;
  std::string                 z;
};

TEST_CASE("MPI Test")
{
  mpi::environment environment;

  static_assert(mpi::is_compliant_v<aggregate>, "Aggregate failed compliance.");
  //static_assert(!mpi::is_compliant_v<non_aggregate>, "Non-aggregate failed compliance.");
}