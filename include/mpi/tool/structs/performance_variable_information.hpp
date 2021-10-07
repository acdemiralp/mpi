#pragma once

#include <string>
#include <optional>

#include <mpi/core/data_type.hpp>
#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/performance_variable_type.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/enum_information.hpp>

namespace mpi::tool
{
struct performance_variable_information
{
  std::string                     name            ;
  std::string                     description     ;

  bind_type                       bind_type       ;
  performance_variable_type       type            ;
  verbosity                       verbosity       ;
  bool                            read_only       ;
  bool                            continuous      ;
  bool                            atomic          ;

  data_type                       data_type       ;
  std::optional<enum_information> enum_information;

};
}