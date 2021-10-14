#pragma once

#include <cstddef>
#include <span>
#include <type_traits>

namespace mpi
{
template <typename type>
struct is_span : std::false_type {};
template <typename type, std::size_t size>
struct is_span<std::span<type, size>> : std::true_type {};

template <typename type, typename = void>
struct is_dynamic_span : std::false_type {};
template <typename type, std::size_t size>
struct is_dynamic_span<std::span<type, size>, std::enable_if_t<size == std::dynamic_extent>> : std::true_type {};

template <typename type, typename = void>
struct is_static_span : std::false_type {};
template <typename type, std::size_t size>
struct is_static_span <std::span<type, size>, std::enable_if_t<size != std::dynamic_extent>> : std::true_type {};

template <typename type>
inline constexpr bool is_span_v         = is_span<type>::value;
template <typename type>
inline constexpr bool is_dynamic_span_v = is_dynamic_span<type>::value;
template <typename type>
inline constexpr bool is_static_span_v  = is_static_span<type>::value;
}