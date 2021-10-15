#pragma once

#include <cstddef>
#include <type_traits>

#include <mpi/core/utility/complex_traits.hpp>
#include <mpi/core/utility/span_traits.hpp>
#include <mpi/third_party/pfr.hpp>

// There are four data type categories:
//
// 1- Compliant types:
//    - Arithmetic types, enumerations and complexes are compliant types.
//    - Static arrays, static spans, pairs, tuples, and aggregates consisting of other compliant types are also compliant types.
//    - Each compliant type has a corresponding MPI data type.
//
// 2- Associative containers:
//    - Specifically: std::map, std::multimap, std::set, std::multiset, std::unordered_map, std::unordered_multimap, std::unordered_set, std::unordered_multiset.
//    - Each associative container has the following functions: .begin(), .end(), .size().
//    - They do not have a corresponding MPI data type, yet can be used with MPI assuming their elements are compliant types.
//    - The elements are first copied to a contiguous container using .begin() and .end().
//    - It is then possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
//
// 3- Non-contiguous sequential containers:
//    - Specifically: std::deque, std::forward_list, std::list, std::vector<bool>.
//    - Each non-contiguous sequential container has the following functions: .begin(), .end(), .size().
//    - They do not have a corresponding MPI data type, yet can be used with MPI assuming their elements are compliant types.
//    - The elements are first copied to a contiguous container using .begin() and .end().
//    - It is then possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
//
// 4- Contiguous sequential containers:
//    - Specifically: std::basic_string, std::span, std::valarray, std::vector<!bool>.
//    - Each contiguous sequential container has the following functions: .begin(), .end(), .data(), .size().
//    - They do not have a corresponding MPI data type, yet can be used with MPI assuming their elements are compliant types.
//    - It is possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
namespace mpi
{
template <typename type, typename = void>
struct is_compliant : std::false_type {};

template <typename type, typename = void>
struct is_compliant_array : std::false_type {};
template <typename type, std::size_t size>
struct is_compliant_array<type[size]            , std::enable_if_t<is_compliant<type>::value>> : std::true_type {};
template <typename type, std::size_t size>
struct is_compliant_array<std::array<type, size>, std::enable_if_t<is_compliant<type>::value>> : std::true_type {};

template <typename type, typename = void>
struct is_compliant_span : std::false_type {};
template <typename type, std::size_t size>
struct is_compliant_span<std::span<type, size>, std::enable_if_t<std::conjunction_v<is_bounded_span<std::span<type, size>>, is_compliant<type>>>> : std::true_type {};

template <typename type, typename = void>
struct is_compliant_tuple : std::false_type {};
template <typename... types>
struct is_compliant_tuple<std::tuple<types...>     , std::enable_if_t<std::conjunction_v<is_compliant<types>...>>>                    : std::true_type {};
template <typename first, typename second>
struct is_compliant_tuple<std::pair <first, second>, std::enable_if_t<std::conjunction_v<is_compliant<first>, is_compliant<second>>>> : std::true_type {};

template <typename type, typename = void>
struct is_compliant_aggregate : std::false_type {};
#ifdef MPI_RELAXED_TRAITS
template <typename type>
struct is_compliant_aggregate<type, std::enable_if_t<std::conjunction_v<std::is_aggregate<type>>>> : std::true_type {};
#else
// The pfr::structure_to_tuple triggers static asserts if the type is an aggregate but not a simple aggregate.
// While this forces the user to pass valid types to the traits, it prevents querying validity of traits at runtime.
// Hence it can be disabled by defining MPI_RELAXED_TRAITS.
// See https://github.com/boostorg/pfr/issues/56 for a future alternative.
template <typename type>
struct is_compliant_aggregate<type, std::enable_if_t<std::conjunction_v<std::is_aggregate<type>, is_compliant_tuple<decltype(pfr::structure_to_tuple(type()))>>>> : std::true_type {};
#endif

template <typename type>
struct is_compliant<type, std::enable_if_t<std::disjunction_v<
  std::is_arithmetic    <type>,
  std::is_enum          <type>,
  is_complex            <type>,
  is_compliant_array    <type>,
  is_compliant_span     <type>,
  is_compliant_tuple    <type>,
  is_compliant_aggregate<type>>>> : std::true_type {};

template <typename type>
inline constexpr bool is_compliant_array_v     = is_compliant_array    <type>::value;
template <typename type>
inline constexpr bool is_compliant_span_v      = is_compliant_span     <type>::value;
template <typename type>
inline constexpr bool is_compliant_tuple_v     = is_compliant_tuple    <type>::value;
template <typename type>
inline constexpr bool is_compliant_aggregate_v = is_compliant_aggregate<type>::value;
template <typename type>
inline constexpr bool is_compliant_v           = is_compliant          <type>::value;

// TODO: BYTE ARRAYS.
}