#pragma once

#include <cstdint>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/type/data_type.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class status
{
public:
  status           (const MPI_Status& native = {}) : native_(native)
  {
    
  }
  status           (const status&  that) = default;
  status           (      status&& temp) = default;
 ~status           ()                    = default;
  status& operator=(const status&  that) = default;
  status& operator=(      status&& temp) = default;

  [[nodiscard]]
  std::int32_t count              (const data_type& type) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Get_count, (&native_, type.native(), &result))
    MPI_CHECK_UNDEFINED (MPI_Get_count, result)
    return result;
  }

  [[nodiscard]]
  std::int32_t element_count      (const data_type& type) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Get_elements, (&native_, type.native(), &result))
    MPI_CHECK_UNDEFINED (MPI_Get_elements, result)
    return result;
  }
  void         set_element_count  (const data_type& type, const std::int32_t count)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_elements, (&native_, type.native(), count))
  }

  [[nodiscard]]
  mpi::count   element_count_x    (const data_type& type) const
  {
    mpi::count result;
    MPI_CHECK_ERROR_CODE(MPI_Get_elements_x, (&native_, type.native(), &result))
    MPI_CHECK_UNDEFINED (MPI_Get_elements_x, result)
    return result;
  }
  void         set_element_count_x(const data_type& type, const mpi::count   count)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_elements_x, (&native_, type.native(), count))
  }

  [[nodiscard]]
  bool         cancelled          () const
  {
    auto result(0);
    MPI_CHECK_ERROR_CODE(MPI_Test_cancelled, (&native_, &result))
    return static_cast<bool>(result);
  }
  void         set_cancelled      (const bool cancelled)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_cancelled, (&native_, cancelled))
  }

  [[nodiscard]]
  std::int32_t source             () const
  {
    return native_.MPI_SOURCE;
  }
  [[nodiscard]]
  std::int32_t tag                () const
  {
    return native_.MPI_TAG;
  }
  [[nodiscard]]
  error_code   error              () const
  {
    return error_code(native_.MPI_ERROR);
  }

  [[nodiscard]]
  MPI_Status   native             () const
  {
    return native_;
  }

protected:
  MPI_Status native_;
};
}