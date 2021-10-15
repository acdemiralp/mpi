#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace mpi
{
template <typename>
inline constexpr bool is_pair_v                            = false;
template <typename first, typename second>
inline constexpr bool is_pair_v<std::pair<first, second>>  = true ;

template <typename>
inline constexpr bool is_tuple_v                           = false;
template <typename... type>
inline constexpr bool is_tuple_v<std::tuple<type...>>      = true ;
template <typename first, typename second>
inline constexpr bool is_tuple_v<std::pair<first, second>> = true ; // "... A pair is a specific case of a std::tuple with two elements. ..."

template <class type>
struct is_pair  : std::bool_constant<is_pair_v <type>> {};
template <class type>
struct is_tuple : std::bool_constant<is_tuple_v<type>> {};

template<typename function_type, typename tuple_type>
void tuple_for_each(function_type&& function, tuple_type&& tuple)
{
  std::apply([&function] (auto&&... value) { (function(value), ...); }, tuple);
}

template <typename type>
concept pair  = is_pair_v <type>;
template <typename type>
concept tuple = is_tuple_v<type>;
}