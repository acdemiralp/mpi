#pragma once

#include <cstdint>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/type/data_type.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class status : public MPI_Status
{
public:
  status           () : MPI_Status {}
  {

  }
  status           (const MPI_Status& native) : MPI_Status(native)
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
    MPI_CHECK_ERROR_CODE(MPI_Get_count, (this, type.native(), &result))
    MPI_CHECK_UNDEFINED (MPI_Get_count, result)
    return result;
  }

  [[nodiscard]]
  std::int32_t element_count      (const data_type& type) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Get_elements, (this, type.native(), &result))
    MPI_CHECK_UNDEFINED (MPI_Get_elements, result)
    return result;
  }
  void         set_element_count  (const data_type& type, const std::int32_t count)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_elements, (this, type.native(), count))
  }

  [[nodiscard]]
  mpi::count   element_count_x    (const data_type& type) const
  {
    mpi::count result;
    MPI_CHECK_ERROR_CODE(MPI_Get_elements_x, (this, type.native(), &result))
    MPI_CHECK_UNDEFINED (MPI_Get_elements_x, result)
    return result;
  }
  void         set_element_count_x(const data_type& type, const mpi::count   count)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_elements_x, (this, type.native(), count))
  }

  [[nodiscard]]
  bool         cancelled          () const
  {
    auto result(0);
    MPI_CHECK_ERROR_CODE(MPI_Test_cancelled, (this, &result))
    return static_cast<bool>(result);
  }
  void         set_cancelled      (const bool cancelled)
  {
    MPI_CHECK_ERROR_CODE(MPI_Status_set_cancelled, (this, cancelled))
  }

  [[nodiscard]]
  std::int32_t source             () const
  {
    return MPI_SOURCE;
  }
  [[nodiscard]]
  std::int32_t tag                () const
  {
    return MPI_TAG;
  }
  [[nodiscard]]
  error_code   error              () const
  {
    return error_code(MPI_ERROR);
  }
};
}