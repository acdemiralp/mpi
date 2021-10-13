#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

namespace mpi
{
template <typename type>
struct is_array : std::is_array<type> {};

template <typename type, std::size_t size>
struct is_array<std::array<type, size>> : std::true_type {};

template <typename type>
inline constexpr bool is_array_v = is_array<type>::value;
}