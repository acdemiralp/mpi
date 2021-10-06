#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
class performance_variable
{
public:
  explicit performance_variable  ()
  {

  }
  performance_variable           (const performance_variable&  that) = default;
  performance_variable           (      performance_variable&& temp) = default;
  virtual ~performance_variable  ()
  {

  }
  performance_variable& operator=(const performance_variable&  that) = default;
  performance_variable& operator=(      performance_variable&& temp) = default;
};
}