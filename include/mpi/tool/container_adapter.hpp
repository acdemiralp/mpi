#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace mpi::tool
{
template <typename type>
struct container_adapter
{
  static void  resize(type& container                   , const std::size_t size)
  {
    // Do nothing.
  }
  static void* data  (type& container)
  {
    return &container;
  }
};
template <typename type, std::size_t size>
struct container_adapter<std::array<type, size>>
{
  static void  resize(std::array<type, size>& container , const std::size_t _size)
  {
    // Do nothing.
  }
  static void* data  (std::array<type, size>& container)
  {
    return container.data();
  }
};
template <typename type>
struct container_adapter<std::vector<type>>
{
  static void  resize(std::vector<type>& container      , const std::size_t size)
  {
    container.resize(size);
  }
  static void* data  (std::vector<type>& container)
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
  static void* data  (std::basic_string<type>& container)
  {
    return container.data();
  }
};
}