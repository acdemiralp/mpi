#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <mpi/core/enums/comparison.hpp>
#include <mpi/core/structs/range.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/session.hpp>

namespace mpi
{
namespace io
{
class file;
}

class group
{
public:
  enum class operation
  {
    difference  ,
    intersection,
    union_
  };

  group             ()
  : native_(MPI_GROUP_EMPTY)
  {

  }
  explicit group    (const MPI_Group native , const bool managed = false)
  : managed_(managed), native_(native)
  {
    
  }
#ifdef MPI_USE_LATEST
  explicit group    (const session& session, const std::string& process_set_name)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Group_from_session_pset, (session.native(), process_set_name.c_str(), &native_))
  }
#endif
  group             (const group&  that, const std::vector<std::int32_t>& ranks , const bool include = true)
  : managed_(true)
  {
    if (include)
      MPI_CHECK_ERROR_CODE(MPI_Group_incl, (that.native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Group_excl, (that.native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), &native_))
  }
  group             (const group&  that, const std::vector<range>&        ranges, const bool include = true)
  : managed_(true)
  {
    if (include)
      MPI_CHECK_ERROR_CODE(MPI_Group_range_incl, (that.native_, static_cast<std::int32_t>(ranges.size()), reinterpret_cast<std::int32_t(*)[3]>(const_cast<std::vector<range>&>(ranges).data()), &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Group_range_excl, (that.native_, static_cast<std::int32_t>(ranges.size()), reinterpret_cast<std::int32_t(*)[3]>(const_cast<std::vector<range>&>(ranges).data()), &native_))
  }
  group             (const group&  lhs , const group& rhs, const operation operation = operation::union_)
  : managed_(true)
  {
    if      (operation == operation::difference)
      MPI_CHECK_ERROR_CODE(MPI_Group_difference  , (lhs.native_, rhs.native_, &native_))
    else if (operation == operation::intersection)
      MPI_CHECK_ERROR_CODE(MPI_Group_intersection, (lhs.native_, rhs.native_, &native_))
    else if (operation == operation::union_)
      MPI_CHECK_ERROR_CODE(MPI_Group_union       , (lhs.native_, rhs.native_, &native_))
  }
  group             (const group&  that) = delete;
  group             (      group&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_GROUP_NULL;
  }
  virtual ~group    () noexcept(false)
  {
    if (managed_ && native_ != MPI_GROUP_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Group_free, (&native_))
  }
  group&  operator= (const group&  that) = delete;
  group&  operator= (      group&& temp) noexcept(false)
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_GROUP_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Group_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_GROUP_NULL;
    }
    return *this;
  }

  bool    operator==(const group&  that) const
  {
    return compare(that) == comparison::identical;
  }

  // Convenience for operation constructor.
  group operator+ (const group&  that) const
  {
    return { *this, that, operation::union_ };
  }
  group operator- (const group&  that) const
  {
    return { *this, that, operation::difference };
  }
  group operator| (const group&  that) const
  {
    return { *this, that, operation::union_ };
  }
  group operator& (const group&  that) const
  {
    return { *this, that, operation::intersection };
  }

  [[nodiscard]]
  std::int32_t              rank           () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Group_rank, (native_, &result))
    // MPI_CHECK_UNDEFINED (MPI_Group_rank, result) // MPI_UNDEFINED should not cause an exception in this case.
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
    MPI_CHECK_ERROR_CODE (MPI_Group_translate_ranks, (native_, static_cast<std::int32_t>(ranks.size()), ranks.data(), that.native_, result.data()))
    // for (auto& value : result)
    //   MPI_CHECK_UNDEFINED(MPI_Group_translate_ranks, value) // MPI_UNDEFINED should not cause an exception in this case.
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
  friend class communicator;
  friend class window;
  friend class io::file;

  bool      managed_ = false;
  MPI_Group native_  = MPI_GROUP_NULL;
};
}