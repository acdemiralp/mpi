#pragma once

#include <cstddef>
#include <span>
#include <type_traits>

namespace mpi
{
template <typename>
inline constexpr bool is_span_v                                                 = false;
template <typename type, std::size_t size>
inline constexpr bool is_span_v<std::span<type, size>>                          = true ;

template <typename>
inline constexpr bool is_bounded_span_v                                         = false;
template <typename type, std::size_t size>
inline constexpr bool is_bounded_span_v  <std::span<type, size>>                = size != std::dynamic_extent;

template <typename>
inline constexpr bool is_unbounded_span_v                                       = false;
template <typename type>
inline constexpr bool is_unbounded_span_v<std::span<type, std::dynamic_extent>> = true ;

template <typename type>
struct is_span           : std::bool_constant<is_span_v          <type>> {};
template <typename type>
struct is_bounded_span   : std::bool_constant<is_bounded_span_v  <type>> {};
template <typename type>
struct is_unbounded_span : std::bool_constant<is_unbounded_span_v<type>> {};

template <typename type>
concept span           = is_span_v          <type>;
template <typename type>
concept bounded_span   = is_bounded_span_v  <type>;
template <typename type>
concept unbounded_span = is_unbounded_span_v<type>;
}