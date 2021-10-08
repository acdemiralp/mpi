#pragma once

#include <cstdint>

namespace mpi
{
class window
{
public:
  explicit window(const MPI_Win native)
  : native_(native)
  {
    
  }

protected:
  bool         managed_ = false;
  std::int32_t native_  = MPI_WIN_NULL;
};
}