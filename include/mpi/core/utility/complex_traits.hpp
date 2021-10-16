#pragma once

#include <complex>
#include <type_traits>

namespace mpi
{
template <typename, typename = void>
inline constexpr bool is_complex_v                     = false;
template <typename type>                       
inline constexpr bool is_complex_v<std::complex<type>> = std::disjunction_v<std::is_same<type, float>, std::is_same<type, double>, std::is_same<type, long double>>;

template <class type>
struct is_complex : std::bool_constant<is_complex_v<type>> {};

template <typename type>
concept complex = is_complex_v<type>;
}