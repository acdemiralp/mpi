#pragma once

#include <variant>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/error/communicator_error_handler.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/op.hpp>
#include <mpi/core/request.hpp>
#include <mpi/core/type/data_type.hpp>
#include <mpi/core/window.hpp>
#include <mpi/core/message.hpp>
#include <mpi/core/information.hpp>
#include <mpi/io/file.hpp>

namespace mpi::tool
{
using object_variant = std::variant<
  communicator ,
  data_type    ,
  communicator_error_handler,
  file         ,
  group        ,
  op           ,
  request      ,
  window       ,
  message      ,
  information  >;
}