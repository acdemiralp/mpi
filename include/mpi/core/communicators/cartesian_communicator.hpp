#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include <mpi/core/communicators/topological_communicator.hpp>
#include <mpi/core/structs/dimension.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class cartesian_communicator : public topological_communicator
{
public:
  explicit cartesian_communicator   (const MPI_Comm native, const bool managed = false)
  : topological_communicator(native, managed)
  {

  }
  cartesian_communicator            (const communicator&            that, const std::vector<bool>&      periodic  , const bool reorder)
  : topological_communicator(MPI_COMM_NULL, true)
  {
    std::vector<std::int32_t> sizes(periodic.size()), periods(periodic.size());
    std::ranges::transform(periodic, periods.begin(), [ ] (const bool value)
    {
      return static_cast<std::int32_t>(value);
    });
    
    MPI_CHECK_ERROR_CODE(MPI_Dims_create, (that.size  (), static_cast<std::int32_t>(periodic.size()), sizes.data()))

    MPI_CHECK_ERROR_CODE(MPI_Cart_create, (that.native(), static_cast<std::int32_t>(periodic.size()), sizes.data(), periods.data(), reorder, &native_))
  }
  cartesian_communicator            (const communicator&            that, const std::vector<dimension>& dimensions, const bool reorder)
  : topological_communicator(MPI_COMM_NULL, true)
  {
    std::vector<std::int32_t> sizes(dimensions.size()), periods(dimensions.size());
    for (std::size_t i = 0; i < dimensions.size(); ++i)
    {
      sizes  [i] = dimensions[i].size;
      periods[i] = static_cast<std::int32_t>(dimensions[i].periodic);
    }

    MPI_CHECK_ERROR_CODE(MPI_Cart_create, (that.native(), static_cast<std::int32_t>(dimensions.size()), sizes.data(), periods.data(), reorder, &native_))
  }
  cartesian_communicator            (const cartesian_communicator&  that, const std::vector<bool>&      keep)
  : topological_communicator(MPI_COMM_NULL, true)
  {
    std::vector<std::int32_t> remain_dims(keep.size());
    std::ranges::transform(keep, remain_dims.begin(), [ ] (const bool value)
    {
      return static_cast<std::int32_t>(value);
    });

    MPI_CHECK_ERROR_CODE(MPI_Cart_sub, (that.native_, remain_dims.data(), &native_))
  }
  cartesian_communicator            (const cartesian_communicator&  that)          = default;
  cartesian_communicator            (      cartesian_communicator&& temp) noexcept = default;
 ~cartesian_communicator            ()                                    override = default;
  cartesian_communicator& operator= (const cartesian_communicator&  that)          = default;
  cartesian_communicator& operator= (      cartesian_communicator&& temp) noexcept = default;

  bool                    operator==(const cartesian_communicator& that) const
  {
    return compare(that) == comparison::identical;
  }

  [[nodiscard]]
  std::int32_t                incoming_neighbor_count() const override
  {
    return 2 * dimension_count();
  }
  [[nodiscard]]
  std::int32_t                outgoing_neighbor_count() const override
  {
    return 2 * dimension_count();
  }

  [[nodiscard]]
  std::int32_t                dimension_count        () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Cartdim_get, (native_, &result))
    return result;
  }
  [[nodiscard]]
  std::vector<dimension>      dimensions             () const
  {
    const auto count = dimension_count();

    std::vector<std::int32_t> sizes (count), periods(count), coordinates(count); // Coordinates unused. Call coordinates() explicitly.
    std::vector<dimension>    result(count);

    MPI_CHECK_ERROR_CODE(MPI_Cart_get, (native_, count, sizes.data(), periods.data(), coordinates.data()))

    std::transform(sizes.begin(), sizes.end(), periods.begin(), result.begin(), [ ] (const std::int32_t size, const std::int32_t periodic)
    {
      return dimension {size, static_cast<bool>(periodic)};
    });

    return result;
  }
  [[nodiscard]]
  std::int32_t                map                    (const std::vector<dimension>& dimensions) const
  {
    std::vector<std::int32_t> sizes(dimensions.size()), periods(dimensions.size());
    for (std::size_t i = 0; i < dimensions.size(); ++i)
    {
      sizes  [i] = dimensions[i].size;
      periods[i] = static_cast<std::int32_t>(dimensions[i].periodic);
    }

    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Cart_map, (native_, static_cast<std::int32_t>(dimensions.size()), sizes.data(), periods.data(), &result))
    // MPI_CHECK_UNDEFINED (MPI_Cart_map, result) // MPI_UNDEFINED should not cause an exception in this case.
    return result;
  }
  
  [[nodiscard]]
  std::int32_t                rank                   (const std::vector<std::int32_t>& coordinates) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Cart_rank, (native_, coordinates.data(), &result))
    return result;
  }
  [[nodiscard]]
  std::vector<std::int32_t>   coordinates            (const std::int32_t rank) const
  {
    std::vector<std::int32_t> result(dimension_count());
    MPI_CHECK_ERROR_CODE(MPI_Cart_coords, (native_, rank, static_cast<std::int32_t>(result.size()), result.data()))
    return result;
  }
  [[nodiscard]]
  std::vector<std::int32_t>   coordinates            () const
  {
    return coordinates(communicator::rank());
  }

  [[nodiscard]]
  std::array<std::int32_t, 2> shift                  (const std::int32_t dimension, const std::int32_t displacement = 1) const
  {
    std::array<std::int32_t, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Cart_shift, (native_, dimension, displacement, &result[0], &result[1]))
    return result;
  }
};
}