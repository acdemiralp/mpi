#pragma once

#include <type_traits>
#include <valarray>
#include <vector>

#include <mpi/core/type/data_type_category.hpp>
#include <mpi/core/type/data_type_reflection.hpp>

namespace mpi
{
template <typename type, typename = void>
struct container_adapter;

template <typename type>
struct container_adapter<type, std::enable_if_t<is_compliant_v                    <type>>>
{
  static void        resize(type& container, const std::size_t size)
  {
    // Do nothing. Compliant types are not resizable.
  }
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
struct container_adapter<type, std::enable_if_t<is_associative_container_v     <type>>>
{
  static void        resize(type& container, const std::size_t size)
  {
    // Do nothing. Associative containers are not resizable.
  }
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
struct container_adapter<type, std::enable_if_t<is_non_contiguous_sequential_container_v     <type>>>
{
  static void        resize(type& container, const std::size_t size)
  {
    container.resize(size);
  }
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
struct container_adapter<type, std::enable_if_t<is_contiguous_sequential_container_v         <type>>>
{
  static void        resize(type& container, const std::size_t size)
  {
    container.resize(size);
  }
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

//template <typename type>
//struct container_adapter<type, std::enable_if_t<is_fixed_size_contiguous_sequential_container_v<type>>>
//{
//  static void        resize(type& container, const std::size_t size)
//  {
//    // Do nothing. Fixed size containers are not resizable.
//  }
//  static std::size_t size  (type& container)
//  {
//    return container.size();
//  }
//  static type*       data  (type& container)
//  {
//    return container.data();
//  }
//};
}