#include "internal/doctest.h"

#include <array>
#include <cstdint>
#include <span>
#include <string>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/type/data_type_reflection.hpp>
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

  REQUIRE( mpi::is_compliant<float         >::value);
  REQUIRE(!mpi::is_compliant<std::nullptr_t>::value);
                                                                                            
  REQUIRE(!mpi::is_compliant_static_span<float                                               >::value);
  REQUIRE(!mpi::is_compliant_static_span<std::nullptr_t                                      >::value);
  REQUIRE( mpi::is_compliant_static_span<std::span<float, 10                                >>::value);
  REQUIRE(!mpi::is_compliant_static_span<std::span<float, std::dynamic_extent               >>::value);
  //REQUIRE( mpi::is_compliant_static_span<std::span<std::span<float, 10>,                  10>>::value);
  //REQUIRE(!mpi::is_compliant_static_span<std::span<std::span<float, std::dynamic_extent>, 10>>::value);
  //REQUIRE(!mpi::is_compliant_static_span<std::span<std::string, 10                 >>::value);
  //REQUIRE(!mpi::is_compliant_static_span<std::span<std::string, std::dynamic_extent>>::value);
  //REQUIRE( mpi::is_compliant            <std::span<float, 10                 >>::value);
  //REQUIRE(!mpi::is_compliant            <std::span<float, std::dynamic_extent>>::value);

  REQUIRE(!mpi::is_compliant_tuple<float                                >::value);
  REQUIRE(!mpi::is_compliant_tuple<std::nullptr_t                       >::value);
  REQUIRE(!mpi::is_compliant_tuple<std::span<float, 10                 >>::value);
  REQUIRE(!mpi::is_compliant_tuple<std::span<float, std::dynamic_extent>>::value);
  REQUIRE( mpi::is_compliant_tuple<std::tuple<char, int, float         >>::value);
  REQUIRE(!mpi::is_compliant_tuple<std::tuple<char, int, std::string   >>::value);

  //REQUIRE(std::is_aggregate<std::string>::value);
  //REQUIRE( mpi::is_compliant_aggregate<aggregate    >::value);
  //REQUIRE(!mpi::is_compliant_aggregate<non_aggregate>::value);
  //REQUIRE( mpi::is_compliant          <aggregate    >::value);
  //REQUIRE(!mpi::is_compliant          <non_aggregate>::value);
}