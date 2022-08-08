#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/event_source.hpp>

namespace mpi::tool
{
#ifdef MPI_USE_LATEST
class event_instance
{
public:
  explicit event_instance  (const MPI_T_event_instance native, const std::size_t copy_buffer_size = 8)
  : native_(native), copy_buffer_size_(copy_buffer_size)
  {
    
  }
  event_instance           (const event_instance&  that) = default;
  event_instance           (      event_instance&& temp) = default;
  virtual ~event_instance  ()                            = default;
  event_instance& operator=(const event_instance&  that) = default;
  event_instance& operator=(      event_instance&& temp) = default;

  [[nodiscard]]
  count                timestamp       () const
  {
    count result;
    MPI_CHECK_ERROR_CODE(MPI_T_event_get_timestamp, (native_, &result))
    return result;
  }
  [[nodiscard]]
  std::int32_t         source_index    () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_T_event_get_source   , (native_, &result))
    return result;
  }

  template <typename type = std::vector<std::byte>> [[nodiscard]]
  type                 copy            () const
  {
    type result;
    container_adapter<type>::resize(result, copy_buffer_size_);
    MPI_CHECK_ERROR_CODE(MPI_T_event_copy, (native_, container_adapter<type>::data(result))) // MPI deficit: Should just return size, rather than forcing user to forward it from event info to handle to instance...
    return result;
  }
  // The type must match the ith data type of the event.
  template <typename type> [[nodiscard]]
  type                 read            (const std::int32_t index) const
  {
    type result;
    container_adapter<type>::resize(result, 1); // MPI deficit: No size data available for string types.
    MPI_CHECK_ERROR_CODE(MPI_T_event_read, (native_, index, container_adapter<type>::data(result)))
    return result;
  }

  // Convenience.
  [[nodiscard]]
  event_source         source          () const
  {
    return event_source(source_index());
  }

  [[nodiscard]]
  MPI_T_event_instance native          () const
  {
    return native_;
  }
  [[nodiscard]]
  std::size_t          copy_buffer_size() const
  {
    return copy_buffer_size_;
  }

protected:
  MPI_T_event_instance native_;
  std::size_t          copy_buffer_size_;
};
#endif
}
