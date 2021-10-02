#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>
#include <mpi/io/access_mode.hpp>

namespace mpi
{
class file
{
public:
  file           ()
  {

  }
  file           (const file&  that) = delete;
  file           (      file&& temp) noexcept
  {
    
  }
  virtual ~file  ()
  {
    
  }
  file& operator=(const file&  that) = delete;
  file& operator=(      file&& temp) noexcept
  {
    
  }

protected:
  MPI_File native_;
};
}