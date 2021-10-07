#pragma once

#include <cstdint>
#include <string>

namespace mpi::tool
{
struct enum_item
{
  explicit enum_item  (const MPI_T_enum native, const std::int32_t index)
  {
    auto name_length(0);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_item, (native, index, &value, nullptr    , &name_length))

    name.resize(name_length);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_item, (native, index, &value, name.data(), &name_length))
  }
  enum_item           (const enum_item&  that) = default;
  enum_item           (      enum_item&& temp) = default;
  virtual ~enum_item  ()                       = default;
  enum_item& operator=(const enum_item&  that) = default;
  enum_item& operator=(      enum_item&& temp) = default;

  std::string  name ;
  std::int32_t value;
};
}