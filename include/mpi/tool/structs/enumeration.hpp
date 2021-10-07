#pragma once

#include <string>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/enumeration_item.hpp>

namespace mpi::tool
{
struct enumeration
{
  explicit enumeration  (const MPI_T_enum native)
  {
    auto item_count(0), name_length(0);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_info, (native, &item_count, nullptr    , &name_length))

    name.resize(name_length);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_info, (native, &item_count, name.data(), &name_length))

    items.reserve(item_count);
    for (auto i = 0; i < item_count; ++i)
      items.emplace_back(native, i);
  }
  enumeration           (const enumeration&  that) = default;
  enumeration           (      enumeration&& temp) = default;
  virtual ~enumeration  ()                         = default;
  enumeration& operator=(const enumeration&  that) = default;
  enumeration& operator=(      enumeration&& temp) = default;

  std::string                   name ;
  std::vector<enumeration_item> items;
};
}