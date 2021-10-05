#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include <mpi/core/enums/comparison.hpp>
#include <mpi/core/structs/range.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class group
{
public:
  enum class filter
  {
    exclude,
    include
  };
  enum class operation
  {
    difference  ,
    intersection,
    union_
  };

  explicit group   (const MPI_Group native)
  : native_(native)
  {
    
  }
  group            (const group&  that, const std::vector<std::int32_t>& ranks , const filter filter = filter::include)
  : managed_(true)
  {
    if      (filter == filter::exclude)
      MPI_CHECK_ERROR_CODE(MPI_Group_excl, (that.native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), &native_))
    else if (filter == filter::include)
      MPI_CHECK_ERROR_CODE(MPI_Group_incl, (that.native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), &native_))
  }
  group            (const group&  that, const std::vector<range>&        ranges, const filter filter = filter::include)
  : managed_(true)
  {
    if      (filter == filter::exclude)
      MPI_CHECK_ERROR_CODE(MPI_Group_range_excl, (that.native_, static_cast<std::int32_t>(ranges.size()), reinterpret_cast<std::int32_t(*)[3]>(const_cast<std::vector<range>&>(ranges).data()), &native_))
    else if (filter == filter::include)
      MPI_CHECK_ERROR_CODE(MPI_Group_range_incl, (that.native_, static_cast<std::int32_t>(ranges.size()), reinterpret_cast<std::int32_t(*)[3]>(const_cast<std::vector<range>&>(ranges).data()), &native_))
  }
  group            (const group&  lhs , const group& rhs, const operation operation = operation::union_)
  : managed_(true)
  {
    if      (operation == operation::difference)
      MPI_CHECK_ERROR_CODE(MPI_Group_difference  , (lhs.native_, rhs.native_, &native_))
    else if (operation == operation::intersection)
      MPI_CHECK_ERROR_CODE(MPI_Group_intersection, (lhs.native_, rhs.native_, &native_))
    else if (operation == operation::union_)
      MPI_CHECK_ERROR_CODE(MPI_Group_union       , (lhs.native_, rhs.native_, &native_))
  }
  group            (const group&  that) = delete;
  group            (      group&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_GROUP_NULL;
  }
  virtual ~group   ()
  {
    if (managed_ && native_ != MPI_GROUP_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Group_free, (&native_));
  }
  group& operator= (const group&  that) = delete;
  group& operator= (      group&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_GROUP_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Group_free, (&native_));

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_GROUP_NULL;
    }
    return *this;
  }

  bool   operator==(const group&  that) const
  {
    return compare(that) == comparison::identical;
  }
  group  operator+ (const group&  that) const
  {
    return group(*this, that, operation::union_);
  }
  group  operator- (const group&  that) const
  {
    return group(*this, that, operation::difference);
  }
  group  operator& (const group&  that) const
  {
    return group(*this, that, operation::intersection);
  }

  [[nodiscard]]
  std::int32_t              rank           () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Group_rank, (native_, &result))
    return result;
  }
  [[nodiscard]]
  std::int32_t              size           () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Group_size, (native_, &result))
    return result;
  }

  [[nodiscard]]
  comparison                compare        (const group& that) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Group_compare, (native_, that.native_, &result))
    return static_cast<comparison>(result);
  }
  [[nodiscard]]
  std::vector<std::int32_t> translate_ranks(const std::vector<std::int32_t>& ranks, const group& that) const
  {
    std::vector<std::int32_t> result(ranks.size());
    MPI_CHECK_ERROR_CODE(MPI_Group_translate_ranks, (native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), that.native_, result.data()))
    return result;
  }

  [[nodiscard]]
  bool                      managed        () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Group                 native         () const
  {
    return native_;
  }

protected:
  bool      managed_ = false;
  MPI_Group native_  = MPI_GROUP_NULL;
};
}