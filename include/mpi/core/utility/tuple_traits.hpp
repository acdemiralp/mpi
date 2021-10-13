#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace mpi
{
template <typename type>
struct is_tuple : std::false_type {};

template <typename... type>
struct is_tuple<std::tuple<type...>> : std::true_type {};

template <typename first, typename second>
struct is_tuple<std::pair<first, second>> : std::true_type {}; // "A pair is a specific case of a std::tuple with two elements."

template <typename type>
inline constexpr bool is_tuple_v = is_tuple<type>::value;

template<typename tuple_type, typename function_type>
void tuple_for_each(function_type&& function, tuple_type&& tuple)
{
  std::apply([&function] (auto&&... value) { (function(value), ...); }, tuple);
}
}