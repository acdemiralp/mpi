#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
class category
{
public:
  explicit category  ()
  {

  }
  category           (const category&  that) = default;
  category           (      category&& temp) = default;
  virtual ~category  ()
  {

  }
  category& operator=(const category&  that) = default;
  category& operator=(      category&& temp) = default;
};
}