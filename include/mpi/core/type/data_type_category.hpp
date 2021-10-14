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
#include <mpi/core/utility/tuple_traits.hpp>

// There are four data type categories:
//
// 1- Basic Types:
//    - Arithmetic types and enumerations are basic types.
//    - Arrays, pairs, tuples, and aggregates consisting of other basic types are also basic types.
//    - Each basic type has a corresponding MPI data type.
//
// 2- Non-contiguous Containers:
//    - STL containers which do not have a contiguous memory layout are non-contiguous containers.
//    - Specifically: std::deque, std::forward_list, std::list, std::vector<bool>, std::map, std::multimap, std::set, std::multiset, std::unordered_map, std::unordered_multimap, std::unordered_set, std::unordered_multiset.
//    - Non-contiguous containers do not have a corresponding MPI data type.
//    - Assuming its elements are basic types, a non-contiguous container can nevertheless be used with MPI.
//    - The elements are first copied to a contiguous container using .begin() and .end(). It is then possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
//
// 3- Contiguous Containers:
//    - STL containers which have a contiguous memory layout are contiguous containers.
//    - Specifically: std::vector, std::valarray.
//    - Contiguous containers do not have a corresponding MPI data type.
//    - Assuming its elements are basic types, a contiguous container can nevertheless be used with MPI.
//    - It is possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
//
// 4- Constant Contiguous Containers:
//    - STL containers which have a contiguous memory layout and only provide a constant reference .data() function are constant contiguous containers.
//    - Specifically: std::basic_string, std::span.
//    - Constant contiguous containers do not have a corresponding MPI data type.
//    - Assuming its elements are basic types, a constant contiguous container can nevertheless be used with MPI.
//    - It is possible to obtain the MPI data type of the value_type, and pass it along with &[0] and .size() to MPI functions.
namespace mpi
{
// TODO: ARRAYS, TUPLES, AGGREGATES SHOULD ONLY CONSIST OF BASICS TO BE IS_BASIC.
template<typename type>
struct is_basic : std::integral_constant<bool, std::is_arithmetic_v<type> || std::is_enum_v<type> || is_array_v<type> || is_tuple_v<type> || std::is_aggregate_v<type>> {}; 

template <typename type>
struct is_non_contiguous_container                                                                                 : std::false_type {};
template <typename type                                                            , typename allocator>
struct is_non_contiguous_container     <std::deque             <type                                 , allocator>> : std::true_type  {};
template <typename type                                                            , typename allocator>
struct is_non_contiguous_container     <std::forward_list      <type                                 , allocator>> : std::true_type  {};
template <typename type                                                            , typename allocator>
struct is_non_contiguous_container     <std::list              <type                                 , allocator>> : std::true_type  {};
template <                                                                           typename allocator>
struct is_non_contiguous_container     <std::vector            <bool                                 , allocator>> : std::true_type  {};
template <typename key_type, typename value_type, typename compare                 , typename allocator>
struct is_non_contiguous_container     <std::map               <key_type, value_type, compare        , allocator>> : std::true_type  {};
template <typename key_type, typename value_type, typename compare                 , typename allocator>
struct is_non_contiguous_container     <std::multimap          <key_type, value_type, compare        , allocator>> : std::true_type  {};
template <typename type                         , typename compare                 , typename allocator>
struct is_non_contiguous_container     <std::set               <type                , compare        , allocator>> : std::true_type  {};
template <typename type                         , typename compare                 , typename allocator>
struct is_non_contiguous_container     <std::multiset          <type                , compare        , allocator>> : std::true_type  {};
template <typename key_type, typename value_type, typename hash, typename key_equal, typename allocator>
struct is_non_contiguous_container     <std::unordered_map     <key_type, value_type, hash, key_equal, allocator>> : std::true_type  {};
template <typename key_type, typename value_type, typename hash, typename key_equal, typename allocator>
struct is_non_contiguous_container     <std::unordered_multimap<key_type, value_type, hash, key_equal, allocator>> : std::true_type  {};
template <typename type                         , typename hash, typename key_equal, typename allocator>
struct is_non_contiguous_container     <std::unordered_set     <type                , hash, key_equal, allocator>> : std::true_type  {};
template <typename type                         , typename hash, typename key_equal, typename allocator>
struct is_non_contiguous_container     <std::unordered_multiset<type                , hash, key_equal, allocator>> : std::true_type  {};

template <typename type>
struct is_contiguous_container                                                                                     : std::false_type {};
template <typename type, typename allocator>
struct is_contiguous_container         <std::vector  <type, allocator>>                                            : std::true_type  {};
template <typename type>
struct is_contiguous_container         <std::valarray<type>>                                                       : std::true_type  {};

template <typename type>
struct is_constant_contiguous_container                                                                            : std::false_type {};
template <typename type, typename traits, typename allocator>
struct is_constant_contiguous_container<std::basic_string<type, traits, allocator>>                                : std::true_type  {};
template <typename type, std::size_t extent>
struct is_constant_contiguous_container<std::span        <type, extent>>                                           : std::true_type  {};

template <typename type>
inline constexpr bool is_basic_v                         = is_basic                        <type>::value;
template <typename type>
inline constexpr bool is_non_contiguous_container_v      = is_non_contiguous_container     <type>::value;
template <typename type>
inline constexpr bool is_contiguous_container_v          = is_contiguous_container         <type>::value;
template <typename type>
inline constexpr bool is_constant_contiguous_container_v = is_constant_contiguous_container<type>::value;
}