#pragma once

#include <type_traits>
#include <valarray>
#include <vector>

#include <mpi/core/type/compliant_container_traits.hpp>
#include <mpi/core/type/type_traits.hpp>

// Container adapter is responsible for routing data from raw objects and STL containers to MPI and vice verse.
// It takes a different action for the four data type categories:
// - is_compliant
// - is_compliant_associative_container
// - is_compliant_non_contiguous_sequential_container
// - is_compliant_contiguous_sequential_container
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
struct container_adapter<type, std::enable_if_t<is_compliant_associative_container_v<type>>>
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
struct container_adapter<type, std::enable_if_t<is_compliant_non_contiguous_sequential_container_v<type>>>
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
struct container_adapter<type, std::enable_if_t<is_compliant_contiguous_sequential_container_v<type>>>
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