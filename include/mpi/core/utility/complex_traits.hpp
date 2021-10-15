#pragma once

#include <complex>
#include <type_traits>

namespace mpi
{
template <typename>
inline constexpr bool is_complex_v                     = false;
template <typename type>                       
inline constexpr bool is_complex_v<std::complex<type>> = true ;

template <class type>
struct is_complex : std::bool_constant<is_complex_v<type>> {};
}