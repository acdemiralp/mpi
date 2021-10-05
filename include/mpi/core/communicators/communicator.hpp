#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include <mpi/core/enums/comparison.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class communicator
{
public:
  explicit communicator   (const MPI_Comm       native)
  : native_(native)
  {
    
  }
  communicator            (const communicator&  that, const group&       group      )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create       , (that.native_, group      .native(), &native_))
  }
  communicator            (const communicator&  that, const information& information)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_dup_with_info, (that.native_, information.native(), &native_))
  }
  communicator            (const communicator&  that)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_dup, (that.native_, &native_))
  }
  communicator            (      communicator&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_COMM_NULL;
  }
  virtual ~communicator   ()
  {
    if (managed_ && native_ != MPI_COMM_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Comm_free, (&native_))
  }
  communicator& operator= (const communicator&  that)
  {
    if (this != &that)
    {
      if (managed_ && native_ != MPI_COMM_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Comm_free, (&native_))

      managed_ = true;
      MPI_CHECK_ERROR_CODE(MPI_Comm_dup, (that.native_, &native_))
    }
    return *this;
  }
  communicator& operator= (      communicator&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_COMM_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Comm_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_COMM_NULL;
    }
    return *this;
  }

  bool          operator==(const communicator&  that) const
  {
    return compare(that) == comparison::identical;
  }

  [[nodiscard]]
  std::int32_t rank           () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_rank, (native_, &result))
    return result;
  }
  [[nodiscard]]
  std::int32_t size           () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_size, (native_, &result))
    return result;
  }

  [[nodiscard]]
  std::string  name           () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, ' ');
    std::int32_t length(0);
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_name, (native_, &result[0], &length))
    result.resize(static_cast<std::size_t>(length));
    return result;
  }
  void         set_name       (const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_name, (native_, value.c_str()))
  }

  [[nodiscard]]
  information  information    () const
  {
    MPI_Info result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_info, (native_, &result))
    return mpi::information(result);
  }
  void         set_information(const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_info, (native_, value.native()))
  }

  void         abort          (const std::int32_t error_code) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Abort, (native_, error_code))
  }

  void         barrier        () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Barrier, (native_))
  }

  [[nodiscard]]
  comparison   compare        (const communicator& that) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_compare, (native_, that.native_, &result))
    return static_cast<comparison>(result);
  }

  [[nodiscard]]
  bool         managed        () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Comm     native         () const
  {
    return native_;
  }

protected:
  communicator() = default; // Default constructor is only available to sub classes who control the member variables explicitly.
  
  bool     managed_ = false;
  MPI_Comm native_  = MPI_COMM_NULL;
};

inline const communicator world_communicator(MPI_COMM_WORLD);
inline const communicator self_communicator (MPI_COMM_SELF );
}