#pragma once

#include <string>
#include <optional>

#include <mpi/core/data_type.hpp>
#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/performance_variable_class.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/enum_information.hpp>

namespace mpi::tool
{
struct performance_variable_information
{
  std::string                     name            ;
  std::string                     description     ;
  verbosity                       verbosity       ;
  performance_variable_class      variable_class  ;
  data_type                       type            ;
  std::optional<enum_information> enum_information;
  bind_type                       bind_type       ;
  bool                            read_only       ;
  bool                            continuous      ;
  bool                            atomic          ;
};
}