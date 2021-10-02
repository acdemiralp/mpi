#pragma once

#include <type_traits>

namespace mpi
{
template<typename type>
struct is_bitset_enum
{
  static const bool enable = false;
};
}

template<typename type>
typename std::enable_if<mpi::is_bitset_enum<type>::enable, type >::type  operator & (const type&  lhs, const type& rhs)
{
  using underlying = typename std::underlying_type<type>::type;
  return static_cast<type> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}
template<typename type>
typename std::enable_if<mpi::is_bitset_enum<type>::enable, type >::type  operator ^ (const type&  lhs, const type& rhs)
{
  using underlying = typename std::underlying_type<type>::type;
  return static_cast<type> (static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}
template<typename type>
typename std::enable_if<mpi::is_bitset_enum<type>::enable, type >::type  operator | (const type&  lhs, const type& rhs)
{
  using underlying = typename std::underlying_type<type>::type;
  return static_cast<type> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}
template<typename type>
typename std::enable_if<mpi::is_bitset_enum<type>::enable, type >::type  operator ~ (const type&  lhs)
{
  using underlying = typename std::underlying_type<type>::type;
  return static_cast<type> (~static_cast<underlying>(lhs));
}

template<typename type>
typename std::enable_if<mpi::is_bitset_enum<type>::enable, type >::type& operator &=(type& lhs, const type& rhs)
{
  using underlying = typename std::underlying_type<type>::type;
  lhs = static_cast<type> (static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
  return lhs;
}
template<typename type>
typename std::enable_if<mpi::is_bitset_enum<type>::enable, type >::type& operator ^=(type& lhs, const type& rhs)
{
  using underlying = typename std::underlying_type<type>::type;
  lhs = static_cast<type> (static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
  return lhs;
}
template<typename type>
typename std::enable_if<mpi::is_bitset_enum<type>::enable, type&>::type  operator |=(type& lhs, const type& rhs)
{
  using underlying = typename std::underlying_type<type>::type;
  lhs = static_cast<type> (static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
  return lhs;
}
