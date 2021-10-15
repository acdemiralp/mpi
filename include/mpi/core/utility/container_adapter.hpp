#pragma once

#include <type_traits>
#include <valarray>
#include <vector>

#include <mpi/core/type/compliant_container_traits.hpp>
#include <mpi/core/type/type_traits.hpp>

// Container adapter provides a uniform interface for accessing different data type categories.
//
// - For basic compliant types:
//   - Stateless.
//   - Exposes .size() = 1 and .data() = &object for read and write operations.
//
// - For compliant associative containers and non contiguous sequential containers:
//   - Stateful: Copies the contents of the container to a vector<container::value_type> using .begin() and .end().
//   - Exposes .size() and .data() of the vector for read and write operations.
//
// - For compliant contiguous sequential containers:
//   - Stateless.
//   - Exposes .size() and .data() of the container for read and write operations.
//
// To support the weakest link, container adapters should be used in a stateful manner.
namespace mpi
{
template <typename type, typename = void>
struct container_adapter;

template <typename type>
struct container_adapter<type, std::enable_if_t<is_compliant_v<type>>>
{
  static std::size_t size  (type& container)
  {
    return 1;
  }
  static type*       data  (type& container)
  {
    return &container;
  }
};

template <typename type>
struct container_adapter<type, std::enable_if_t<std::conjunction_v<std::negation<is_compliant<type>>, std::disjunction<is_compliant_associative_container<type>, is_compliant_non_contiguous_sequential_container<type>>>>>
{
  static std::size_t size  (type& container)
  {
    return container.size();
  }
  static type*       data  (type& container)
  {
    // TODO: Copy the elements to a contiguous buffer and return a pointer to that instead!
    std::vector<typename type::value_type> contiguous(container.begin(), container.end());
  }
};

template <typename type>
struct container_adapter<type, std::enable_if_t<std::conjunction_v<std::negation<is_compliant<type>>, is_compliant_contiguous_sequential_container<type>>>>
{
  static std::size_t size  (type& container)
  {
    return container.size();
  }
  static type*       data  (type& container)
  {
    if constexpr (std::is_same_v<type, std::valarray<typename type::value_type>>)
      return &container[0];  // std::valarray does not have a .data() function.
    else
      return container.data();
  }
};
}