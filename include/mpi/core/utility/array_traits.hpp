#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

// Extended with std::array.
namespace mpi
{
template <typename>
inline constexpr bool is_array_v                                 = false;
template <typename type, std::size_t size>                       
inline constexpr bool is_array_v<type[size]>                     = true ;
template <typename type>                                         
inline constexpr bool is_array_v<type[]>                         = true ;
template <typename type, std::size_t size>                       
inline constexpr bool is_array_v<std::array<type, size>>         = true ;

template <typename>
inline constexpr bool is_bounded_array_v                         = false;
template <typename type, std::size_t size>
inline constexpr bool is_bounded_array_v<type[size]>             = true ;
template <typename type, std::size_t size>
inline constexpr bool is_bounded_array_v<std::array<type, size>> = true ;

template <typename>
inline constexpr bool is_unbounded_array_v                       = false;
template <typename type>
inline constexpr bool is_unbounded_array_v<type[]>               = true ;

template <class type>
struct is_array           : std::bool_constant<is_array_v          <type>> {};
template <class type>
struct is_bounded_array   : std::bool_constant<is_bounded_array_v  <type>> {};
template <class type>
struct is_unbounded_array : std::bool_constant<is_unbounded_array_v<type>> {};

template <typename type>
concept array           = is_array_v          <type>;
template <typename type>
concept bounded_array   = is_bounded_array_v  <type>;
template <typename type>
concept unbounded_array = is_unbounded_array_v<type>;
}