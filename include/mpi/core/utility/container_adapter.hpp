#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <valarray>
#include <vector>

namespace mpi
{
template <typename type>
struct container_adapter
{
  static void  resize(type& container, const std::size_t size)
  {
    // Do nothing.
  }
  static type* data  (type& container)
  {
    return &container;
  }
};
template <typename type, std::size_t size>
struct container_adapter<type[size]>
{
  static void  resize(type (&container) [size], const std::size_t _size)
  {
    // Do nothing.
  }
  static type* data  (type (&container) [size])
  {
    return container;
  }
};

template <typename type, std::size_t size>
struct container_adapter<std::array<type, size>>
{
  static void  resize(std::array<type, size>& container, const std::size_t _size)
  {
    // Do nothing.
  }
  static type* data  (std::array<type, size>& container)
  {
    return container.data();
  }
};
template <typename type>
struct container_adapter<std::valarray<type>>
{
  static void  resize(std::valarray<type>& container, const std::size_t size)
  {
    container.resize(size);
  }
  static type* data  (std::valarray<type>& container)
  {
    return &container[0];
  }
};
template <typename type>
struct container_adapter<std::vector<type>>
{
  static void  resize(std::vector<type>& container, const std::size_t size)
  {
    container.resize(size);
  }
  static type* data  (std::vector<type>& container)
  {
    return container.data();
  }
};

template <typename type>
struct container_adapter<std::basic_string<type>>
{
  static void  resize(std::basic_string<type>& container, const std::size_t size)
  {
    container.resize(size);
  }
  static type* data  (std::basic_string<type>& container)
  {
    return &container[0];
  }
};

template <typename type, std::size_t extent>
struct container_adapter<std::span<type, extent>>
{
  static void  resize(std::span<type, extent>& container, const std::size_t size)
  {
    // Do nothing.
  }
  static type* data  (std::span<type, extent>& container)
  {
    return &container[0];
  }
};

// The following are omitted due to not being contiguous:
// - Sequence              containers: std::deque, std::forward_list, std::list
// - Associative           containers: std::set, std::map, std::multiset, std::multimap
// - Unordered associative containers: std::unordered_set, std::unordered_map, std::unordered_multiset, std::unordered_multimap.
}