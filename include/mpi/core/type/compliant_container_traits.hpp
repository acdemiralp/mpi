#pragma once

#include <type_traits>

#include <mpi/core/type/compliant_traits.hpp>
#include <mpi/core/utility/container_traits.hpp>

namespace mpi
{
template <typename type>
inline constexpr bool is_compliant_associative_container_v               = std::conjunction_v<is_associative_container              <type>, is_compliant<typename type::value_type>>;
template <typename type>
inline constexpr bool is_compliant_non_contiguous_sequential_container_v = std::conjunction_v<is_non_contiguous_sequential_container<type>, is_compliant<typename type::value_type>>;
// Arrays are not compliant contiguous sequential containers, they are compliant types (furthermore C arrays do not have a value_type).
template <typename type>
inline constexpr bool is_compliant_contiguous_sequential_container_v     = std::conjunction_v<is_contiguous_sequential_container    <type>, std::negation<is_array<type>>, is_compliant<typename type::value_type>>;

template <typename type>
struct is_compliant_associative_container               : std::bool_constant<is_compliant_associative_container_v              <type>> {};
template <typename type>
struct is_compliant_non_contiguous_sequential_container : std::bool_constant<is_compliant_non_contiguous_sequential_container_v<type>> {};
template <typename type>
struct is_compliant_contiguous_sequential_container     : std::bool_constant<is_compliant_contiguous_sequential_container_v    <type>> {};

template <typename type>
concept compliant_associative_container               = is_compliant_associative_container_v              <type>;
template <typename type>
concept compliant_non_contiguous_sequential_container = is_compliant_non_contiguous_sequential_container_v<type>;
template <typename type>
concept compliant_contiguous_sequential_container     = is_compliant_contiguous_sequential_container_v    <type>;
}