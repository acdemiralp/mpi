#pragma once

#include <mpi/core/utility/associative_container_traits.hpp>
#include <mpi/core/utility/sequential_container_traits.hpp>

namespace mpi
{
template <typename type>
inline constexpr bool is_container_v = std::disjunction_v<is_associative_container<type>, is_sequential_container<type>>;

template <class type>
struct is_container : std::bool_constant<is_container_v<type>> {};
}