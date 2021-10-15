#pragma once

#include <map>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace mpi
{
template <typename>
inline constexpr bool is_ordered_associative_container_v = false;
template <typename key_type, typename value_type, typename compare, typename allocator>
inline constexpr bool is_ordered_associative_container_v<std::map     <key_type, value_type, compare, allocator>> = true;
template <typename key_type, typename value_type, typename compare, typename allocator>
inline constexpr bool is_ordered_associative_container_v<std::multimap<key_type, value_type, compare, allocator>> = true;
template <typename type, typename compare, typename allocator>
inline constexpr bool is_ordered_associative_container_v<std::set     <type, compare, allocator>> = true;
template <typename type, typename compare, typename allocator>
inline constexpr bool is_ordered_associative_container_v<std::multiset<type, compare, allocator>> = true;

template <class type>
struct is_ordered_associative_container : std::bool_constant<is_ordered_associative_container_v<type>> {};

template <typename>
inline constexpr bool is_unordered_associative_container_v = false;
template <typename key_type, typename value_type, typename hash, typename key_equal, typename allocator>
inline constexpr bool is_unordered_associative_container_v<std::unordered_map     <key_type, value_type, hash, key_equal, allocator>> = true;
template <typename key_type, typename value_type, typename hash, typename key_equal, typename allocator>
inline constexpr bool is_unordered_associative_container_v<std::unordered_multimap<key_type, value_type, hash, key_equal, allocator>> = true;
template <typename type, typename hash, typename key_equal, typename allocator>
inline constexpr bool is_unordered_associative_container_v<std::unordered_set     <type, hash, key_equal, allocator>> = true;
template <typename type, typename hash, typename key_equal, typename allocator>
inline constexpr bool is_unordered_associative_container_v<std::unordered_multiset<type, hash, key_equal, allocator>> = true;

template <class type>
struct is_unordered_associative_container : std::bool_constant<is_unordered_associative_container_v<type>> {};

template <typename type>
inline constexpr bool is_associative_container_v = std::disjunction_v<is_ordered_associative_container<type>, is_unordered_associative_container<type>>;

template <class type>
struct is_associative_container : std::bool_constant<is_associative_container_v<type>> {};

template <typename type>
concept ordered_associative_container   = is_ordered_associative_container_v  <type>;
template <typename type>
concept unordered_associative_container = is_unordered_associative_container_v<type>;
template <typename type>
concept associative_container           = is_associative_container_v          <type>;
}