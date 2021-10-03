#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>
#include <mpi/io/enums/access_mode.hpp>

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

  [[nodiscard]]
  std::int64_t size() const
  {
    std::int64_t result;
    MPI_File_get_size(native_, &result);
    return result;
  }

protected:
  MPI_File native_;
};
}