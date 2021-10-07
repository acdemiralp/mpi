#pragma once

#include <string>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/enum_item.hpp>

namespace mpi::tool
{
struct enum_information
{
  explicit enum_information  (const MPI_T_enum native)
  {
    auto item_count(0), name_length(0);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_info, (native, &item_count, nullptr    , &name_length))

    name.resize(name_length);

    MPI_CHECK_ERROR_CODE(MPI_T_enum_get_info, (native, &item_count, name.data(), &name_length))

    items.reserve(item_count);
    for (auto i = 0; i < item_count; ++i)
      items.emplace_back(native, i);
  }
  enum_information           (const enum_information&  that) = default;
  enum_information           (      enum_information&& temp) = default;
  virtual ~enum_information  ()                              = default;
  enum_information& operator=(const enum_information&  that) = default;
  enum_information& operator=(      enum_information&& temp) = default;

  std::string            name ;
  std::vector<enum_item> items;
};
}