#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/structs/process_set.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
#ifdef MPI_USE_LATEST
class session
{
public:
  session           (const information& information, const session_error_handler& error_handler)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Session_init, (information.native(), error_handler.native(), &native_))
  }
  explicit session  (const MPI_Session  native, const bool managed = false)
  : managed_(managed), native_(native)
  {
    
  }
  session           (const session&     that  ) = delete;
  session           (      session&&    temp  ) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_SESSION_NULL;
  }
  virtual ~session  ()
  {
    if (managed_ && native_ != MPI_SESSION_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Session_finalize, (&native_))
  }
  session& operator=(const session&     that  ) = delete;
  session& operator=(      session&&    temp  ) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_SESSION_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Session_finalize, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_SESSION_NULL;
    }
    return *this;
  }

  [[nodiscard]]
  std::int32_t             process_set_count      (                          const information& information = mpi::information()) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Session_get_num_psets, (native_, information.native(), &result))
    return result;
  }
  [[nodiscard]]
  std::string              process_set_name       (const std::int32_t index, const information& information = mpi::information()) const
  {
    std::int32_t size  (0);
    MPI_CHECK_ERROR_CODE(MPI_Session_get_nth_pset, (native_, information.native(), index, &size, nullptr   ))

    std::string  result(size, ' ');
    MPI_CHECK_ERROR_CODE(MPI_Session_get_nth_pset, (native_, information.native(), index, &size, &result[0]))
    return result;
  }
  [[nodiscard]]
  information              process_set_information(const std::string& name) const
  {
    mpi::information result(MPI_INFO_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Session_get_pset_info, (native_, name.c_str(), &result.native_))
    return result;
  }

  // Convenience for process sets.
  [[nodiscard]]
  std::int32_t             process_set_size       (const std::string& name) const
  {
    const auto info = process_set_information(name);
    return std::stoi(*info["mpi_size"]);
  }
  [[nodiscard]]
  std::vector<process_set> process_sets           (const information& information = mpi::information()) const
  {
    std::vector<process_set> result(process_set_count(information));
    for (std::size_t i = 0; i < result.size(); ++i)
    {
      const auto name = process_set_name(static_cast<std::int32_t>(i));
      result[i]       = {name, process_set_size(name)};
    }
    return result;
  }

  [[nodiscard]]
  information              information            () const
  {
    mpi::information result(MPI_INFO_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Session_get_info, (native_, &result.native_))
    return result;
  }

  [[nodiscard]]
  session_error_handler    error_handler          () const
  {
    session_error_handler result;
    MPI_CHECK_ERROR_CODE(MPI_Session_get_errhandler , (native_, &result.native_))
    return result;
  }
  void                     set_error_handler      (const window_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Session_set_errhandler , (native_, value.native()))
  }
  void                     call_error_handler     (const error_code&           value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Session_call_errhandler, (native_, value.native()))
  }

  [[nodiscard]]
  bool                     managed                () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Session              native                 () const
  {
    return native_;
  }

protected:
  bool        managed_ = false;
  MPI_Session native_  = MPI_SESSION_NULL;
};
#endif
}