#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
class communicator
{
public:
  communicator           ()
  : MPI_Comm_create()
  {
    
  }
  communicator           (const communicator&  that) = default;
  communicator           (      communicator&& temp) = default;
  virtual ~communicator  ()
  {
    
  }
  communicator& operator=(const communicator&  that) = default;
  communicator& operator=(      communicator&& temp) = default;

protected:
  MPI_Comm native_;
};
}