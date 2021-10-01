#pragma once

#include <cstddef>
#include <cstdint>

#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/return_code.hpp>

namespace mpi
{
class communicator
{
public:
  communicator           (const communicator&  that, const group&       group      )
  : managed_(true)
  {
    MPI_Comm_create(that.native_, group.native(), &native_);
  }
  communicator           (const communicator&  that, const information& information)
  : managed_(true)
  {
    MPI_Comm_dup_with_info(that.native_, information.native(), &native_);
  }
  communicator           (const communicator&  that)
  : managed_(true)
  {
    MPI_Comm_dup(that.native_, &native_);
  }
  communicator           (      communicator&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = 0;
  }
  virtual ~communicator  ()
  {
    if (managed_)
      MPI_Comm_free(&native_);
  }
  communicator& operator=(const communicator&  that)
  {
    managed_ = true;
    MPI_Comm_dup(that.native_, &native_);
    return *this;
  }
  communicator& operator=(      communicator&& temp) noexcept
  {
    if (this != &temp)
    {
      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = 0;
    }
    return *this;
  }

  [[nodiscard]]
  std::int32_t rank           () const
  {
    std::int32_t result;
    MPI_Comm_rank(native_, &result);
    return result;
  }
  [[nodiscard]]
  std::int32_t size           () const
  {
    std::int32_t result;
    MPI_Comm_size(native_, &result);
    return result;
  }

  [[nodiscard]]
  std::string  name           () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, ' ');
    std::int32_t length(0);
    MPI_Comm_get_name(native_, &result[0], &length);
    result.resize(static_cast<std::size_t>(length));
    return result;
  }
  bool         set_name       (const std::string& value) const
  {
    return MPI_Comm_set_name(native_, value.c_str()) == MPI_SUCCESS;
  }

  [[nodiscard]]
  information  information    () const
  {
    MPI_Info result;
    MPI_Comm_get_info(native_, &result);
    return mpi::information(result);
  }
  bool         set_information(const mpi::information& value) const
  {
    return MPI_Comm_set_info(native_, value.native()) == MPI_SUCCESS;
  }

  bool         abort          (std::int32_t return_code) const
  {
    return MPI_Abort(native_, return_code) == MPI_SUCCESS;
  }

  bool         barrier        () const
  {
    return MPI_Barrier(native_) == MPI_SUCCESS;
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
  bool     managed_ = false;
  MPI_Comm native_  = 0;
};
}