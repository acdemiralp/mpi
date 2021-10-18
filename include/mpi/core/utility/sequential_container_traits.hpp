#pragma once

#include <array>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <list>
#include <span>
#include <string>
#include <type_traits>
#include <valarray>
#include <vector>

namespace mpi
{
template <typename>
inline constexpr bool is_contiguous_sequential_container_v                                                 = false;
template <typename type>
inline constexpr bool is_contiguous_sequential_container_v    <type[]>                                     = true ;
template <typename type, std::size_t size>
inline constexpr bool is_contiguous_sequential_container_v    <type[size]>                                 = true ;
template <typename type, std::size_t size>
inline constexpr bool is_contiguous_sequential_container_v    <std::array       <type, size>>              = true ;
template <typename type, typename traits, typename allocator>
inline constexpr bool is_contiguous_sequential_container_v    <std::basic_string<type, traits, allocator>> = true ;
template <typename type, std::size_t size>
inline constexpr bool is_contiguous_sequential_container_v    <std::span        <type, size>>              = true ;
template <typename type>
inline constexpr bool is_contiguous_sequential_container_v    <std::valarray    <type>>                    = true ;
template <typename type, typename allocator>
inline constexpr bool is_contiguous_sequential_container_v    <std::vector      <type, allocator>>         = !std::is_same_v<type, bool>;

template <typename type>
struct is_contiguous_sequential_container : std::bool_constant<is_contiguous_sequential_container_v<type>> {};

template <typename>
inline constexpr bool is_non_contiguous_sequential_container_v                                             = false;
template <typename type, typename allocator>
inline constexpr bool is_non_contiguous_sequential_container_v<std::deque       <type, allocator>>         = true ;
template <typename type, typename allocator>
inline constexpr bool is_non_contiguous_sequential_container_v<std::forward_list<type, allocator>>         = true ;
template <typename type, typename allocator>
inline constexpr bool is_non_contiguous_sequential_container_v<std::list        <type, allocator>>         = true ;
template <typename allocator>
inline constexpr bool is_non_contiguous_sequential_container_v<std::vector      <bool, allocator>>         = true ;

template <typename type>
struct is_non_contiguous_sequential_container : std::bool_constant<is_non_contiguous_sequential_container_v<type>> {};

template <typename type>
inline constexpr bool is_sequential_container_v = std::disjunction_v<is_contiguous_sequential_container<type>, is_non_contiguous_sequential_container<type>>;

template <typename type>
struct is_sequential_container : std::bool_constant<is_sequential_container_v<type>> {};

template <typename type>
concept contiguous_sequential_container     = is_contiguous_sequential_container_v    <type>;
template <typename type>
concept non_contiguous_sequential_container = is_non_contiguous_sequential_container_v<type>;
template <typename type>
concept sequential_container                = is_sequential_container_v               <type>;
}