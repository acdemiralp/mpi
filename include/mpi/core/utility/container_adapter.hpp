#pragma once

#include <cstddef>
#include <type_traits>
#include <valarray>

#include <mpi/core/type/compliant_container_traits.hpp>
#include <mpi/core/type/type_traits.hpp>

// Send and receive, as well as most collectives accept data using the signature `(const) void* buffer, std::int32_t count, MPI_Datatype data_type`
// where the buffer is a C pointer or array to `count` many objects of type `data_type`.
// Container adapters unify the interface to obtain the `buffer`, `count` and `data_type` of single compliant objects and contiguous sequential containers.
namespace mpi
{
template <typename type, typename = void>
class container_adapter;

template <compliant type>
class container_adapter<type>
{
public:
  using value_type = type;

  static data_type   data_type()
  {
    return type_traits<value_type>::get_data_type();
  }
  static value_type* data     (type& container)
  {
    return &container;
  }
  static std::size_t size     (type& container)
  {
    return 1;
  }

  static void        resize   (type& container, const std::size_t size)
  {
    // Do nothing. Compliant types are not resizable.
  }
};

template <compliant_contiguous_sequential_container type>
class container_adapter<type>
{
public:
  using value_type  = typename type::value_type;

  static data_type   data_type()
  {
    return type_traits<value_type>::get_data_type();
  }
  static value_type* data     (type& container)
  {
    if constexpr (std::is_same_v<type, std::valarray<value_type>>)
      return &container[0];  // std::valarray does not have a .data() function.
    else
      return container.data();
  }
  static std::size_t size     (type& container)
  {
    return container.size();
  }
  
  static void        resize   (type& container, const std::size_t size)
  {
    container.resize(size);
  }
};
}