#pragma once

#include <cstdint>

#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/request.hpp>
#include <mpi/core/status.hpp>

namespace mpi
{
class message
{
public:
  explicit message  (const MPI_Message native = MPI_MESSAGE_NULL) : native_(native)
  {
    
  }
  message           (const message&    that) = default;
  message           (      message&&   temp) = default;
  virtual ~message  ()                       = default;
  message& operator=(const message&    that) = default;
  message& operator=(      message&&   temp) = default;

  status      receive          (void* data, const std::int32_t size, const data_type& data_type)
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_Mrecv, (data, size, data_type.native(), &native_, &result))
    return result;
  }
  template <typename type>
  status      receive          (type& data)
  {
    using adapter = container_adapter<type>;
    return receive(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }

  [[nodiscard]]
  request     immediate_receive(void* data, const std::int32_t size, const data_type& data_type)
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Imrecv, (data, size, data_type.native(), &native_, &result.native_))
    return result;
  }
  template <typename type>
  [[nodiscard]]
  request     immediate_receive(type& data)
  {
    using adapter = container_adapter<type>;
    return immediate_receive(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }

  [[nodiscard]]
  MPI_Message native           () const
  {
    return native_;
  }

protected:
  friend class communicator;

  MPI_Message native_;
};
}