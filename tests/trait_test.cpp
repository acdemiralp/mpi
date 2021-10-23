#include "internal/doctest.h"

#include <array>
#include <complex>
#include <cstdint>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <span>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

#define MPI_USE_RELAXED_TRAITS

#include <mpi/all.hpp>

struct aggregate
{
  std::int32_t                x;
  std::array<std::int32_t, 3> y;
};
struct non_aggregate : aggregate
{
  std::string z;
};

struct nested_aggregate
{
  std::int32_t x;
  aggregate    y;
};
struct nested_non_aggregate
{
  std::int32_t  x;
  non_aggregate y;
};

enum class enum_type { x, y, z };

using non_compliant_type                      = std::string;

using arithmetic_compliant_type               = float;

using enum_compliant_type                     = enum_type;

using complex_compliant_type                  = std::complex<float>;
using complex_non_compliant_type              = std::complex<std::uint64_t>;

using static_c_array_compliant_type           = float[4];
using static_c_array_compliant_type_2d        = float[4][4];
using static_c_array_compliant_type_3d        = float[4][4][4];
using static_c_array_compliant_type_4d        = float[4][4][4][4];
using static_c_array_non_compliant_type       = std::string[4];
using static_c_array_non_compliant_type_2d    = std::string[4][4];
using static_c_array_non_compliant_type_3d    = std::string[4][4][4];
using static_c_array_non_compliant_type_4d    = std::string[4][4][4];

using dynamic_c_array_non_compliant_type      = float[ ];
using dynamic_c_array_non_compliant_type_2d   = float[ ][4];
using dynamic_c_array_non_compliant_type_3d   = float[ ][4][4];
using dynamic_c_array_non_compliant_type_4d   = float[ ][4][4][4];
using dynamic_c_array_non_compliant_type_2    = std::string[ ];
using dynamic_c_array_non_compliant_type_2_2d = std::string[ ][4];
using dynamic_c_array_non_compliant_type_2_3d = std::string[ ][4][4];
using dynamic_c_array_non_compliant_type_2_4d = std::string[ ][4][4];

using array_compliant_type                    = std::array<float, 4>;
using array_compliant_type_2d                 = std::array<std::array<float, 4>, 4>;
using array_compliant_type_3d                 = std::array<std::array<std::array<float, 4>, 4>, 4>;
using array_compliant_type_4d                 = std::array<std::array<std::array<std::array<float, 4>, 4>, 4>, 4>;
using array_non_compliant_type                = std::array<std::string, 4>;
using array_non_compliant_type_2d             = std::array<std::array<std::string, 4>, 4>;
using array_non_compliant_type_3d             = std::array<std::array<std::array<std::string, 4>, 4>, 4>;
using array_non_compliant_type_4d             = std::array<std::array<std::array<std::array<std::string, 4>, 4>, 4>, 4>;

using pair_compliant_type                     = std::pair<std::int32_t, std::int32_t>;
using pair_compliant_type_2d                  = std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>;
using pair_non_compliant_type                 = std::pair<std::int32_t, std::string >;
using pair_non_compliant_type_2d              = std::pair<std::int32_t, std::pair<std::int32_t, std::string>>;

using tuple_compliant_type                    = std::tuple<std::int32_t, std::int32_t>;
using tuple_compliant_type_2d                 = std::tuple<std::int32_t, std::tuple<std::int32_t, std::int32_t>>;
using tuple_non_compliant_type                = std::tuple<std::int32_t, std::string >;
using tuple_non_compliant_type_2d             = std::tuple<std::int32_t, std::tuple<std::int32_t, std::string>>;

using aggregate_compliant_type                = aggregate;
using aggregate_compliant_type_2d             = nested_aggregate;
using aggregate_non_compliant_type            = non_aggregate;
using aggregate_non_compliant_type_2d         = nested_non_aggregate;

using map_compliant_type                      = std::map<std::int32_t, std::int32_t>;
using map_compliant_type_2                    = std::map<std::int32_t, aggregate>;
using map_non_compliant_type                  = std::map<std::int32_t, std::string>;
using map_non_compliant_type_2                = std::map<std::int32_t, non_aggregate>;
using unordered_map_compliant_type            = std::unordered_map<std::int32_t, std::int32_t>;
using unordered_map_compliant_type_2          = std::unordered_map<std::int32_t, aggregate>;
using unordered_map_non_compliant_type        = std::unordered_map<std::int32_t, std::string>;
using unordered_map_non_compliant_type_2      = std::unordered_map<std::int32_t, non_aggregate>;

using set_compliant_type                      = std::set<std::int32_t>;
using set_compliant_type_2                    = std::set<aggregate>;
using set_non_compliant_type                  = std::set<std::string>;
using set_non_compliant_type_2                = std::set<non_aggregate>;
using unordered_set_compliant_type            = std::unordered_set<std::int32_t>;
using unordered_set_compliant_type_2          = std::unordered_set<aggregate>;
using unordered_set_non_compliant_type        = std::unordered_set<std::string>;
using unordered_set_non_compliant_type_2      = std::unordered_set<non_aggregate>;

using deque_compliant_type                    = std::deque<std::int32_t>;
using deque_compliant_type_2                  = std::deque<aggregate>;
using deque_non_compliant_type                = std::deque<std::string>;
using deque_non_compliant_type_2              = std::deque<non_aggregate>;
using deque_compliant_type_2d                 = std::deque<std::deque<std::int32_t>>;
using deque_non_compliant_type_2d             = std::deque<std::deque<std::string>>;

using forward_list_compliant_type             = std::forward_list<std::int32_t>;
using forward_list_compliant_type_2           = std::forward_list<aggregate>;
using forward_list_non_compliant_type         = std::forward_list<std::string>;
using forward_list_non_compliant_type_2       = std::forward_list<non_aggregate>;
using forward_list_compliant_type_2d          = std::forward_list<std::forward_list<std::int32_t>>;
using forward_list_non_compliant_type_2d      = std::forward_list<std::forward_list<std::string>>;

using list_compliant_type                     = std::list<std::int32_t>;
using list_compliant_type_2                   = std::list<aggregate>;
using list_non_compliant_type                 = std::list<std::string>;
using list_non_compliant_type_2               = std::list<non_aggregate>;
using list_compliant_type_2d                  = std::list<std::list<std::int32_t>>;
using list_non_compliant_type_2d              = std::list<std::list<std::string>>;

using static_span_compliant_type              = std::span<float      , 4>;
using static_span_non_compliant_type          = std::span<std::string, 4>;
using static_span_non_compliant_type_2d       = std::span<std::span<float      , 4>, 4>;
using static_span_non_compliant_type_2_2d     = std::span<std::span<std::string, 4>, 4>;

using dynamic_span_compliant_type             = std::span<float>;
using dynamic_span_non_compliant_type_2d      = std::span<std::string>;

using valarray_compliant_type                 = std::valarray<std::int32_t>;
using valarray_compliant_type_2               = std::valarray<aggregate>;
using valarray_non_compliant_type             = std::valarray<std::string>;
using valarray_non_compliant_type_2           = std::valarray<non_aggregate>;
using valarray_compliant_type_2d              = std::valarray<std::valarray<std::int32_t>>;
using valarray_non_compliant_type_2d          = std::valarray<std::valarray<std::string>>;

using vector_compliant_type                   = std::vector<std::int32_t>;
using vector_compliant_type_2                 = std::vector<aggregate>;
using vector_non_compliant_type               = std::vector<std::string>;
using vector_non_compliant_type_2             = std::vector<non_aggregate>;
using vector_compliant_type_2d                = std::vector<std::vector<std::int32_t>>;
using vector_non_compliant_type_2d            = std::vector<std::vector<std::string>>;

TEST_CASE("Trait Test")
{
  mpi::environment environment;

  REQUIRE(!mpi::is_compliant_v<non_compliant_type                     >);

  REQUIRE( mpi::is_compliant_v<arithmetic_compliant_type              >);

  REQUIRE( mpi::is_compliant_v<enum_compliant_type                    >);

  REQUIRE( mpi::is_compliant_v<complex_compliant_type                 >);
  REQUIRE(!mpi::is_compliant_v<complex_non_compliant_type             >);

  REQUIRE( mpi::is_compliant_v<static_c_array_compliant_type          >);
  REQUIRE( mpi::is_compliant_v<static_c_array_compliant_type_2d       >);
  REQUIRE( mpi::is_compliant_v<static_c_array_compliant_type_3d       >);
  REQUIRE( mpi::is_compliant_v<static_c_array_compliant_type_4d       >);
  REQUIRE(!mpi::is_compliant_v<static_c_array_non_compliant_type      >);
  REQUIRE(!mpi::is_compliant_v<static_c_array_non_compliant_type_2d   >);
  REQUIRE(!mpi::is_compliant_v<static_c_array_non_compliant_type_3d   >);
  REQUIRE(!mpi::is_compliant_v<static_c_array_non_compliant_type_4d   >);

  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type     >);
  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type_2d  >);
  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type_3d  >);
  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type_4d  >);
  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type_2   >);
  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type_2_2d>);
  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type_2_3d>);
  REQUIRE(!mpi::is_compliant_v<dynamic_c_array_non_compliant_type_2_4d>);

  REQUIRE( mpi::is_compliant_v<array_compliant_type                   >);
  REQUIRE( mpi::is_compliant_v<array_compliant_type_2d                >);
  REQUIRE( mpi::is_compliant_v<array_compliant_type_3d                >);
  REQUIRE( mpi::is_compliant_v<array_compliant_type_4d                >);
  REQUIRE(!mpi::is_compliant_v<array_non_compliant_type               >);
  REQUIRE(!mpi::is_compliant_v<array_non_compliant_type_2d            >);
  REQUIRE(!mpi::is_compliant_v<array_non_compliant_type_3d            >);
  REQUIRE(!mpi::is_compliant_v<array_non_compliant_type_4d            >);

  REQUIRE( mpi::is_compliant_v<pair_compliant_type                    >);
  REQUIRE( mpi::is_compliant_v<pair_compliant_type_2d                 >);
  REQUIRE(!mpi::is_compliant_v<pair_non_compliant_type                >);
  REQUIRE(!mpi::is_compliant_v<pair_non_compliant_type_2d             >);

  REQUIRE( mpi::is_compliant_v<tuple_compliant_type                   >);
  REQUIRE( mpi::is_compliant_v<tuple_compliant_type_2d                >);
  REQUIRE(!mpi::is_compliant_v<tuple_non_compliant_type               >);
  REQUIRE(!mpi::is_compliant_v<tuple_non_compliant_type_2d            >);

  REQUIRE( mpi::is_compliant_v<aggregate_compliant_type               >);
  REQUIRE( mpi::is_compliant_v<aggregate_compliant_type_2d            >);
#ifdef MPI_USE_RELAXED_TRAITS // It is not possible to detect if the inner types are compliant with relaxed traits.
  REQUIRE( mpi::is_compliant_v<aggregate_non_compliant_type           >);
  REQUIRE( mpi::is_compliant_v<aggregate_non_compliant_type_2d        >);
#else
  REQUIRE(!mpi::is_compliant_v<aggregate_non_compliant_type           >);
  REQUIRE(!mpi::is_compliant_v<aggregate_non_compliant_type_2d        >);
#endif

  REQUIRE(!mpi::is_compliant_v<map_compliant_type                     >);
  REQUIRE(!mpi::is_compliant_v<map_compliant_type_2                   >);
  REQUIRE(!mpi::is_compliant_v<map_non_compliant_type                 >);
  REQUIRE(!mpi::is_compliant_v<map_non_compliant_type_2               >);
  REQUIRE(!mpi::is_compliant_v<unordered_map_compliant_type           >);
  REQUIRE(!mpi::is_compliant_v<unordered_map_compliant_type_2         >);
  REQUIRE(!mpi::is_compliant_v<unordered_map_non_compliant_type       >);
  REQUIRE(!mpi::is_compliant_v<unordered_map_non_compliant_type_2     >);

  REQUIRE(!mpi::is_compliant_v<set_compliant_type                     >);
  REQUIRE(!mpi::is_compliant_v<set_compliant_type_2                   >);
  REQUIRE(!mpi::is_compliant_v<set_non_compliant_type                 >);
  REQUIRE(!mpi::is_compliant_v<set_non_compliant_type_2               >);
  REQUIRE(!mpi::is_compliant_v<unordered_set_compliant_type           >);
  REQUIRE(!mpi::is_compliant_v<unordered_set_compliant_type_2         >);
  REQUIRE(!mpi::is_compliant_v<unordered_set_non_compliant_type       >);
  REQUIRE(!mpi::is_compliant_v<unordered_set_non_compliant_type_2     >);

  REQUIRE(!mpi::is_compliant_v<deque_compliant_type                   >);
  REQUIRE(!mpi::is_compliant_v<deque_compliant_type_2                 >);
  REQUIRE(!mpi::is_compliant_v<deque_non_compliant_type               >);
  REQUIRE(!mpi::is_compliant_v<deque_non_compliant_type_2             >);
  REQUIRE(!mpi::is_compliant_v<deque_compliant_type_2d                >);
  REQUIRE(!mpi::is_compliant_v<deque_non_compliant_type_2d            >);

  REQUIRE(!mpi::is_compliant_v<forward_list_compliant_type            >);
  REQUIRE(!mpi::is_compliant_v<forward_list_compliant_type_2          >);
  REQUIRE(!mpi::is_compliant_v<forward_list_non_compliant_type        >);
  REQUIRE(!mpi::is_compliant_v<forward_list_non_compliant_type_2      >);
  REQUIRE(!mpi::is_compliant_v<forward_list_compliant_type_2d         >);
  REQUIRE(!mpi::is_compliant_v<forward_list_non_compliant_type_2d     >);

  REQUIRE(!mpi::is_compliant_v<list_compliant_type                    >);
  REQUIRE(!mpi::is_compliant_v<list_compliant_type_2                  >);
  REQUIRE(!mpi::is_compliant_v<list_non_compliant_type                >);
  REQUIRE(!mpi::is_compliant_v<list_non_compliant_type_2              >);
  REQUIRE(!mpi::is_compliant_v<list_compliant_type_2d                 >);
  REQUIRE(!mpi::is_compliant_v<list_non_compliant_type_2d             >);

  REQUIRE(!mpi::is_compliant_v<static_span_compliant_type             >);
  REQUIRE(!mpi::is_compliant_v<static_span_non_compliant_type         >);
  REQUIRE(!mpi::is_compliant_v<static_span_non_compliant_type_2d      >);
  REQUIRE(!mpi::is_compliant_v<static_span_non_compliant_type_2_2d    >);

  REQUIRE(!mpi::is_compliant_v<dynamic_span_compliant_type            >);
  REQUIRE(!mpi::is_compliant_v<dynamic_span_non_compliant_type_2d     >);

  REQUIRE(!mpi::is_compliant_v<valarray_compliant_type                >);
  REQUIRE(!mpi::is_compliant_v<valarray_compliant_type_2              >);
  REQUIRE(!mpi::is_compliant_v<valarray_non_compliant_type            >);
  REQUIRE(!mpi::is_compliant_v<valarray_non_compliant_type_2          >);
  REQUIRE(!mpi::is_compliant_v<valarray_compliant_type_2d             >);
  REQUIRE(!mpi::is_compliant_v<valarray_non_compliant_type_2d         >);
  
  REQUIRE(!mpi::is_compliant_v<vector_compliant_type                  >);
  REQUIRE(!mpi::is_compliant_v<vector_compliant_type_2                >);
  REQUIRE(!mpi::is_compliant_v<vector_non_compliant_type              >);
  REQUIRE(!mpi::is_compliant_v<vector_non_compliant_type_2            >);
  REQUIRE(!mpi::is_compliant_v<vector_compliant_type_2d               >);
  REQUIRE(!mpi::is_compliant_v<vector_non_compliant_type_2d           >);

  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<map_compliant_type                     >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<map_compliant_type_2                   >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<map_non_compliant_type                 >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<map_non_compliant_type_2               >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_map_compliant_type           >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_map_compliant_type_2         >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_map_non_compliant_type       >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_map_non_compliant_type_2     >);

  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<set_compliant_type                     >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<set_compliant_type_2                   >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<set_non_compliant_type                 >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<set_non_compliant_type_2               >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_set_compliant_type           >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_set_compliant_type_2         >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_set_non_compliant_type       >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<unordered_set_non_compliant_type_2     >);

  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<deque_compliant_type                   >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<deque_compliant_type_2                 >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<deque_non_compliant_type               >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<deque_non_compliant_type_2             >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<deque_compliant_type_2d                >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<deque_non_compliant_type_2d            >);

  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<forward_list_compliant_type            >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<forward_list_compliant_type_2          >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<forward_list_non_compliant_type        >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<forward_list_non_compliant_type_2      >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<forward_list_compliant_type_2d         >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<forward_list_non_compliant_type_2d     >);

  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<list_compliant_type                    >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<list_compliant_type_2                  >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<list_non_compliant_type                >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<list_non_compliant_type_2              >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<list_compliant_type_2d                 >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<list_non_compliant_type_2d             >);

  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<static_span_compliant_type             >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<static_span_non_compliant_type         >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<static_span_non_compliant_type_2d      >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<static_span_non_compliant_type_2_2d    >);

  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<dynamic_span_compliant_type            >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<dynamic_span_non_compliant_type_2d     >);

  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<valarray_compliant_type                >);
  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<valarray_compliant_type_2              >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<valarray_non_compliant_type            >);
#ifdef MPI_USE_RELAXED_TRAITS // It is not possible to detect if the inner types are compliant with relaxed traits.
  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<valarray_non_compliant_type_2          >);
#else
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<valarray_non_compliant_type_2          >);
#endif
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<valarray_compliant_type_2d             >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<valarray_non_compliant_type_2d         >);

  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<vector_compliant_type                  >);
  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<vector_compliant_type_2                >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<vector_non_compliant_type              >);
#ifdef MPI_USE_RELAXED_TRAITS // It is not possible to detect if the inner types are compliant with relaxed traits.
  REQUIRE( mpi::is_compliant_contiguous_sequential_container_v<vector_non_compliant_type_2            >);
#else
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<vector_non_compliant_type_2            >);
#endif
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<vector_compliant_type_2d               >);
  REQUIRE(!mpi::is_compliant_contiguous_sequential_container_v<vector_non_compliant_type_2d           >);
}