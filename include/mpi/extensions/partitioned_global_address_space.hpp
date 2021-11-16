#pragma once

#include <cstddef>
#include <cstdint>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/window.hpp>

namespace mpi
{
// Partitioned global address space extension:
// - Each process holds a partition of the address space.
// - A task over the address space is implemented as local operations followed by a global reduction across processes.
// - Might span the total memory space of all nodes.
// - Perhaps implement in the form of custom allocators for standard containers.
// - Examples: DASH, UPC++.
class partitioned_global_address_space
{
public:
  explicit partitioned_global_address_space  (const communicator& communicator, const std::size_t size_in_bytes = 4e9)
  : window_(communicator, static_cast<std::int64_t>(size_in_bytes))
  {
    
  }
  partitioned_global_address_space           (const partitioned_global_address_space&  that) = delete ;
  partitioned_global_address_space           (      partitioned_global_address_space&& temp) = delete ;
  virtual ~partitioned_global_address_space  ()                                              = default;
  partitioned_global_address_space& operator=(const partitioned_global_address_space&  that) = delete ;
  partitioned_global_address_space& operator=(      partitioned_global_address_space&& temp) = delete ;

  [[nodiscard]]
  const window& window() const
  {
    return window_;
  }

protected:
  mpi::window window_;
};
}