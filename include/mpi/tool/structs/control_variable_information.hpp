#pragma once

#include <string>
#include <optional>

#include <mpi/core/data_type.hpp>
#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/scope.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/enum_information.hpp>

namespace mpi::tool
{
struct control_variable_information
{
  std::string                     name            ;
  std::string                     description     ;
  verbosity                       verbosity       ;
  data_type                       data_type       ;
  std::optional<enum_information> enum_information;
  bind_type                       bind_type       ;
  scope                           scope           ;
};
}