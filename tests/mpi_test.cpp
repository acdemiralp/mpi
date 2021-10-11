#include "internal/doctest.h"

#include <array>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/type/data_type_reflection.hpp>
#include <mpi/core/type/standard_data_types.hpp>

struct type
{
  std::int32_t                x;
  char                        y[256];
  std::array<std::int32_t, 3> z;
};

namespace mpi
{
template <typename type, std::size_t size>
struct type_traits<type[size]>
{
  static data_type get_data_type()
  {
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};
template <typename type, std::size_t size>
struct type_traits<std::array<type, size>>
{
  static data_type get_data_type()
  {
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};

template <>
struct type_traits<type>
{
  static data_type get_data_type()
  {
    std::vector<data_type>    data_types   ;
    std::vector<std::int32_t> block_lengths;
    std::vector<std::int64_t> displacements;

    // Goal: Create a mpi::data_type from a type. Requires reflection to enumerate member variables.

    return std::move(data_type(data_types, block_lengths, displacements));
  }
};
}

TEST_CASE("MPI Test", "[mpi]")
{
  std::vector<mpi::data_type> data_types   (3);
  std::vector<std::int32_t>   block_lengths(3);
  std::vector<std::int64_t>   displacements(3);

  data_types   [0] = mpi::type_traits<std::int32_t>::get_data_type();
  block_lengths[0] = 1;
  displacements[0] = 0;

  data_types   [1] = mpi::type_traits<char[256]>::get_data_type();
  block_lengths[1] = 1;
  displacements[1] = block_lengths[0] * sizeof(std::int32_t);

  data_types   [2] = mpi::type_traits<std::array<std::int32_t, 3>>::get_data_type();
  block_lengths[2] = 1;
  displacements[2] = block_lengths[1] * sizeof(char[256]);

  mpi::data_type data_type(data_types, block_lengths, displacements);
}