#pragma once

#include <type_traits>

namespace mpi
{
// Indicates a missing implementation, for use with static assertions.
template<typename type>
struct missing_implementation : std::false_type { };
}