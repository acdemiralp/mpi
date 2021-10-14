#pragma once

#include <cstddef>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

#include <mpi/core/utility/array_traits.hpp>
#include <mpi/core/utility/span_traits.hpp>
#include <mpi/core/utility/tuple_traits.hpp>
#include <mpi/third_party/pfr.hpp>

// There are five data type categories:
//
// 1- Compliant types:
//    - Arithmetic types and enumerations are compliant types.
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
template<typename type>
struct is_compliant;

template <typename type, typename = void>
struct is_compliant_array : std::false_type {};
template <typename type, std::size_t size>
struct is_compliant_array <type[size]            , std::enable_if_t<is_compliant<type>::value>> : std::true_type {};
template <typename type, std::size_t size>
struct is_compliant_array <std::array<type, size>, std::enable_if_t<is_compliant<type>::value>> : std::true_type {};

template <typename type, typename = void>
struct is_compliant_static_span : std::false_type {};
template <typename type, std::size_t size>
struct is_compliant_static_span <std::span<type, size>, std::enable_if_t<size != std::dynamic_extent && is_compliant<type>::value>> : std::true_type {};

template <typename type, typename = void>
struct is_compliant_tuple : std::false_type {};
template <typename... types>
struct is_compliant_tuple <std::tuple<types...>    , std::enable_if_t<(is_compliant<types>::value && ...)>> : std::true_type {}; // MSVC does not yet support fold expressions in templates (tested and works on GCC 11.2).
template <typename first, typename second>
struct is_compliant_tuple <std::pair<first, second>, std::enable_if_t< is_compliant<first>::value && is_compliant<second>::value>> : std::true_type {}; // "A pair is a specific case of a std::tuple with two elements."

template <typename type, typename = void>
struct is_compliant_aggregate : std::false_type {};
template <typename type>
struct is_compliant_aggregate <type, std::enable_if_t<!std::is_aggregate_v<type>>> : std::false_type {};
template <typename type>
struct is_compliant_aggregate <type, std::enable_if_t<std::conjunction_v<std::is_aggregate<type>, is_compliant_tuple<decltype(pfr::structure_to_tuple(type()))>>>> : std::true_type {};

//  std::is_aggregate<type>::value && !std::is_polymorphic<type>::value && is_compliant_tuple<decltype(pfr::structure_to_tuple(type()))>::value >> : std::true_type{};

template<typename type>
struct is_compliant : std::disjunction<
  std::is_arithmetic      <type> , 
  std::is_enum            <type> , 
  is_compliant_array      <type> ,
  is_compliant_static_span<type> ,
  is_compliant_tuple      <type> ,
  is_compliant_aggregate  <type> > {};

// TODO: SOLVE THE PROBLEM OF ARRAY/SPAN/TUPLE MEMBERS RESOLVING TO AGGREGATE.
// TODO: CONTAINERS SHOULD ALSO HOLD IS_COMPLIANT TYPES.
// TODO: BYTE ARRAYS.

template <typename type>
inline constexpr bool is_compliant_v = is_compliant<type>::value;

// TODO
template<typename type>
struct is_compliant_associative_container : std::integral_constant<bool, is_associative_container<type>::value&& is_compliant<typename type::value_type>::value> {};
}