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

// There are five data type categories:
//
// 1- Compliant types:
//    - Arithmetic types and enumerations are compliant types.
//    - Arrays, static spans, pairs, tuples, and aggregates consisting of other compliant types are also compliant types.
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
//    - Each non-contiguous sequential container has the following functions: .begin(), .end(), .size(), .resize().
//    - They do not have a corresponding MPI data type, yet can be used with MPI assuming their elements are compliant types.
//    - The elements are first copied to a contiguous container using .begin() and .end().
//    - It is then possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
//
// 4- Contiguous sequential containers:
//    - Specifically: std::basic_string, std::valarray, std::vector.
//    - Each contiguous sequential container has the following functions: .begin(), .end(), .data(), .size(), .resize().
//    - They do not have a corresponding MPI data type, yet can be used with MPI assuming their elements are compliant types.
//    - It is possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
//
// 5- Fixed-size contiguous sequential containers:
//    - Specifically: std::span.
//    - Each constant contiguous sequential container has the following functions: .begin(), .end(), .data(), .size().
//    - They do not have a corresponding MPI data type, yet can be used with MPI assuming their elements are compliant types.
//    - It is possible to obtain the MPI data type of the value_type, and pass it along with .data() and .size() to MPI functions.
namespace mpi
{
// TODO: ARRAYS, SPANS, TUPLES, AGGREGATES SHOULD ONLY CONSIST OF COMPLIANTS TO BE IS_COMPLIANT. CONTAINERS SHOULD ALSO HOLD IS_COMPLIANT TYPES.
template<typename type>
struct is_compliant : std::integral_constant<bool,
  std::is_arithmetic_v<type>  || 
  std::is_enum_v      <type>  || 
  is_array_v          <type>  || 
  is_static_span_v    <type>  || 
  is_tuple_v          <type>  || 
  std::is_aggregate_v <type>> {}; 

template <typename type>
struct is_associative_container                                                                                                             : std::false_type {};
template <typename key_type, typename value_type, typename compare                 , typename allocator>                                    
struct is_associative_container                     <std::map               <key_type, value_type, compare        , allocator>>             : std::true_type  {};
template <typename key_type, typename value_type, typename compare                 , typename allocator>                                    
struct is_associative_container                     <std::multimap          <key_type, value_type, compare        , allocator>>             : std::true_type  {};
template <typename type                         , typename compare                 , typename allocator>                                    
struct is_associative_container                     <std::set               <type                , compare        , allocator>>             : std::true_type  {};
template <typename type                         , typename compare                 , typename allocator>                                    
struct is_associative_container                     <std::multiset          <type                , compare        , allocator>>             : std::true_type  {};
template <typename key_type, typename value_type, typename hash, typename key_equal, typename allocator>                                    
struct is_associative_container                     <std::unordered_map     <key_type, value_type, hash, key_equal, allocator>>             : std::true_type  {};
template <typename key_type, typename value_type, typename hash, typename key_equal, typename allocator>                                    
struct is_associative_container                     <std::unordered_multimap<key_type, value_type, hash, key_equal, allocator>>             : std::true_type  {};
template <typename type                         , typename hash, typename key_equal, typename allocator>                                    
struct is_associative_container                     <std::unordered_set     <type                , hash, key_equal, allocator>>             : std::true_type  {};
template <typename type                         , typename hash, typename key_equal, typename allocator>                                    
struct is_associative_container                     <std::unordered_multiset<type                , hash, key_equal, allocator>>             : std::true_type  {};

template <typename type>                                                                                                                    
struct is_non_contiguous_sequential_container                                                                                               : std::false_type {};
template <typename type                                                            , typename allocator>                                    
struct is_non_contiguous_sequential_container       <std::deque             <type                                 , allocator>>             : std::true_type  {};
template <typename type                                                            , typename allocator>                                    
struct is_non_contiguous_sequential_container       <std::forward_list      <type                                 , allocator>>             : std::true_type  {};
template <typename type                                                            , typename allocator>                                    
struct is_non_contiguous_sequential_container       <std::list              <type                                 , allocator>>             : std::true_type  {};
template <                                                                           typename allocator>                                    
struct is_non_contiguous_sequential_container       <std::vector            <bool                                 , allocator>>             : std::true_type  {};

template <typename type>                                                                                                                    
struct is_contiguous_sequential_container                                                                                                   : std::false_type {};
template <typename type, typename traits, typename allocator>                                                                               
struct is_contiguous_sequential_container           <std::basic_string      <type, traits                         , allocator>>             : std::true_type  {};
template <typename type>                                                                                                                    
struct is_contiguous_sequential_container           <std::valarray          <type>>                                                         : std::true_type  {};
template <typename type                                                            , typename allocator>                                    
struct is_contiguous_sequential_container           <std::vector            <type                                 , allocator>>             : std::true_type  {};

template <typename type, typename = void>                                                                                                   
struct is_fixed_size_contiguous_sequential_container                                                                                        : std::false_type {};
template <typename type, std::size_t extent>
struct is_fixed_size_contiguous_sequential_container<std::span<type, extent>, std::enable_if_t<is_dynamic_span_v<std::span<type, extent>>>> : std::true_type  {};

template <typename type>
inline constexpr bool is_compliant_v                                  = is_compliant                                 <type>::value;
template <typename type>
inline constexpr bool is_associative_container_v                      = is_associative_container                     <type>::value;
template <typename type>
inline constexpr bool is_non_contiguous_sequential_container_v        = is_non_contiguous_sequential_container       <type>::value;
template <typename type>
inline constexpr bool is_contiguous_sequential_container_v            = is_contiguous_sequential_container           <type>::value;
template <typename type>
inline constexpr bool is_fixed_size_contiguous_sequential_container_v = is_fixed_size_contiguous_sequential_container<type>::value;
}