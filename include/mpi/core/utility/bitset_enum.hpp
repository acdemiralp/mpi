#pragma once

#include <type_traits>

namespace mpi
{
template<typename type>
struct is_bitset_enum : std::false_type {};

template <typename type>
inline constexpr bool is_bitset_enum_v = is_bitset_enum<type>::value;

template <typename type>
concept bitset_enum = is_bitset_enum_v<type>;
}

template<mpi::bitset_enum type>
type operator & (const type& lhs, const type& rhs)
{
  using underlying = std::underlying_type_t<type>;
  return static_cast<type> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}
template<mpi::bitset_enum type>
type operator ^ (const type& lhs, const type& rhs)
{
  using underlying = std::underlying_type_t<type>;
  return static_cast<type> (static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}
template<mpi::bitset_enum type>
type operator | (const type& lhs, const type& rhs)
{
  using underlying = std::underlying_type_t<type>;
  return static_cast<type> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}
template<mpi::bitset_enum type>
type operator ~ (const type& lhs)
{
  using underlying = std::underlying_type_t<type>;
  return static_cast<type> (~static_cast<underlying>(lhs));
}

template<mpi::bitset_enum type>
type& operator &=(type& lhs, const type& rhs)
{
  using underlying = std::underlying_type_t<type>;
  lhs = static_cast<type> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
  return lhs;
}
template<mpi::bitset_enum type>
type& operator ^=(type& lhs, const type& rhs)
{
  using underlying = std::underlying_type_t<type>;
  lhs = static_cast<type> (static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
  return lhs;
}
template<mpi::bitset_enum type>
type& operator |=(type& lhs, const type& rhs)
{
  using underlying = std::underlying_type_t<type>;
  lhs = static_cast<type> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
  return lhs;
}
