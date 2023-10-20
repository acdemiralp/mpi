#pragma once

#include <cstdint>
#include <string>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
struct enumeration_item
{
  explicit enumeration_item  (const MPI_T_enum native, const std::int32_t index) : index(index)
  {
    auto name_length(0);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_item, (native, index, &value, nullptr    , &name_length))

    name.resize(name_length);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_item, (native, index, &value, name.data(), &name_length))
  }
  enumeration_item           (const enumeration_item&  that) = default;
  enumeration_item           (      enumeration_item&& temp) = default;
  virtual ~enumeration_item  ()                              = default;
  enumeration_item& operator=(const enumeration_item&  that) = default;
  enumeration_item& operator=(      enumeration_item&& temp) = default;

  std::int32_t index   ;
  std::string  name    ;
  std::int32_t value {};
};
}