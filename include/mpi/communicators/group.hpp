#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include <mpi/core/mpi.hpp>

namespace mpi
{
class group
{
public:
  enum class group_filter
  {
    exclude,
    include
  };
  enum class group_operation
  {
    difference  ,
    intersection,
    union_
  };
  enum class comparison : std::int32_t
  {
    identical = MPI_IDENT    ,
    congruent = MPI_CONGRUENT,
    similar   = MPI_SIMILAR  ,
    unequal   = MPI_UNEQUAL
  };
  struct range
  {
    std::int32_t first ;
    std::int32_t last  ;
    std::int32_t stride;
  };

  group                 (const MPI_Group native) : native_(native)
  {
    
  }
  group                 (const group& that, const std::vector<std::int32_t>& ranks , const group_filter filter = group_filter::include)
  {
    if      (filter == group_filter::exclude)
      MPI_Group_excl(that.native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), &native_);
    else if (filter == group_filter::include)
      MPI_Group_incl(that.native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), &native_);
    else
      throw std::runtime_error("Unknown group filter.");
  }
  group                 (const group& that,       std::vector<range>         ranges, const group_filter filter = group_filter::include)
  {
    if      (filter == group_filter::exclude)
      MPI_Group_range_excl(that.native_, static_cast<std::int32_t>(ranges.size()), reinterpret_cast<std::int32_t(*)[3]>(ranges.data()), &native_);
    else if (filter == group_filter::include)
      MPI_Group_range_incl(that.native_, static_cast<std::int32_t>(ranges.size()), reinterpret_cast<std::int32_t(*)[3]>(ranges.data()), &native_);
    else
      throw std::runtime_error("Unknown group filter.");
  }
  group                 (const group& lhs, const group& rhs, const group_operation operation = group_operation::union_)
  {
    if      (operation == group_operation::difference)
      MPI_Group_difference  (lhs.native_, rhs.native_, &native_);
    else if (operation == group_operation::intersection)
      MPI_Group_intersection(lhs.native_, rhs.native_, &native_);
    else if (operation == group_operation::union_)
      MPI_Group_union       (lhs.native_, rhs.native_, &native_);
    else
      throw std::runtime_error("Unknown group operation.");
  }
  group                 (const group&  that) = default;
  group                 (      group&& temp) = default;
  virtual ~group        ()
  {
    MPI_Group_free(&native_);
  }
  group&      operator= (const group&  that) = default;
  group&      operator= (      group&& temp) = default;
  friend bool operator==(const group& lhs, const group& rhs)
  {
    return lhs.compare(rhs) == comparison::identical;
  }

  std::int32_t              rank           () const
  {
    std::int32_t result;
    MPI_Group_rank(native_, &result);
    return result;
  }
  std::int32_t              size           () const
  {
    std::int32_t result;
    MPI_Group_size(native_, &result);
    return result;
  }

  comparison                compare        (const group& that) const
  {
    std::int32_t result;
    MPI_Group_compare(native_, that.native_, &result);
    return static_cast<comparison>(result);
  }
  std::vector<std::int32_t> translate_ranks(const std::vector<std::int32_t>& ranks, const group& that) const
  {
    std::vector<std::int32_t> result(ranks.size());
    MPI_Group_translate_ranks(native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), that.native_, result.data());
    return result;
  }

  std::unique_ptr<group>    range_include  (const std::vector<range>& ranges)
  {
    MPI_Group native;
    auto non_const_range = const_cast<std::vector<range>&>(ranges);
    MPI_Group_range_incl(native_, static_cast<std::int32_t>(ranges.size()), reinterpret_cast<std::int32_t(*)[3]>(non_const_range.data()), &native);
    return group(native_);
  }

protected:
  MPI_Group native_ = 0;
};
}