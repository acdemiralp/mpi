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
  std::string                 y;
  std::array<std::int32_t, 3> z;
};

template <typename type>
mpi::data_type&& get_data_type()
{
  std::vector<mpi::data_type> data_types   ;
  std::vector<std::int32_t>   block_lengths;
  std::vector<std::int64_t>   displacements;

  // Goal: Create a mpi::data_type from a type. Requires reflection to enumerate member variables.

  return std::move(mpi::data_type(data_types, block_lengths, displacements));
}

namespace mpi
{
template <typename type, std::size_t size>
struct type_traits<std::array<type, size>>
{
  data_type get_data_type()
  {
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};

template <>
struct type_traits<type>
{
  data_type get_data_type()
  {
    std::vector<mpi::data_type> data_types   ;
    std::vector<std::int32_t>   block_lengths;
    std::vector<std::int64_t>   displacements;

    // Goal: Create a mpi::data_type from a type. Requires reflection to enumerate member variables.

    return std::move(mpi::data_type(data_types, block_lengths, displacements));
  }
};
}

TEST_CASE("MPI Test", "[mpi]")
{
  std::vector<mpi::data_type> data_types   (3);
  std::vector<std::int32_t>   block_lengths(3);
  std::vector<std::int64_t>   displacements(3);

  data_types   [0] = mpi::data_types::int32_t;
  block_lengths[0] = sizeof(std::int32_t);
  displacements[0] = 0;

  data_types   [1] = mpi::data_types::char_;
  block_lengths[1] = sizeof(std::int32_t);
  displacements[1] = 0;

  data_types   [2] = mpi::data_types::int32_t;
  block_lengths[2] = sizeof(std::int32_t);
  displacements[2] = 0;

  mpi::data_type data_type(data_types, block_lengths, displacements);
}