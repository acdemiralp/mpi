#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/status.hpp>

namespace mpi
{
namespace io
{
class file;
}

class request
{
public:
  explicit request  (const MPI_Request native, const bool managed = false, const bool persistent = false)
  : managed_(managed), native_(native), persistent_(persistent)
  {
    
  }
  request           (const request&    that) = delete;
  request           (      request&&   temp) noexcept
  : managed_(temp.managed_), native_(temp.native_), persistent_(temp.persistent_)
  {
    temp.managed_    = false;
    temp.native_     = MPI_REQUEST_NULL;
    temp.persistent_ = false;
  }
  virtual ~request  ()
  {
    if (managed_ && native_ != MPI_REQUEST_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Request_free, (&native_))
  }
  request& operator=(const request&    that) = delete;
  request& operator=(      request&&   temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_REQUEST_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Request_free, (&native_))

      managed_         = temp.managed_   ;
      native_          = temp.native_    ;
      persistent_      = temp.persistent_;

      temp.managed_    = false           ;
      temp.native_     = MPI_REQUEST_NULL;
      temp.persistent_ = false           ;
    }
    return *this;
  }

  [[nodiscard]]
  std::optional<status> get_status         () const
  {
    std::int32_t complete;
    status       result  ;
    MPI_CHECK_ERROR_CODE(MPI_Request_get_status, (native_, &complete, &result))
    return static_cast<bool>(complete) ? result : std::optional<status>(std::nullopt);
  }
  [[nodiscard]]
  std::optional<status> test               ()
  {
    std::int32_t complete;
    status       result  ;
    MPI_CHECK_ERROR_CODE(MPI_Test, (&native_, &complete, &result))
    return static_cast<bool>(complete) ? result : std::optional<status>(std::nullopt);
  }

  status                wait               ()
  {
    status result;
    MPI_CHECK_ERROR_CODE(MPI_Wait, (&native_, &result))
    return result;
  }

  void                  start              ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Start, (&native_))
  }
  void                  cancel             ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Cancel, (&native_))
  }

#ifdef MPI_USE_LATEST
  bool                  partition_arrived  (const std::int32_t               partition )
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Parrived    , (native_, partition, &result))
    return static_cast<bool>(result);
  }
  void                  set_partition_ready(const std::int32_t               partition ) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Pready      , (partition, native_))
  }
  void                  set_partition_ready(const std::vector<std::int32_t>& partitions) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Pready_list , (static_cast<std::int32_t>(partitions.size()), partitions.data(), native_))
  }
  void                  set_partition_ready(const std::int32_t lower, const std::int32_t upper) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Pready_range, (lower, upper, native_))
  }
#endif

#ifdef MPI_USE_DETACH
  void                  detach             (const std::function<void(const status&)>& function);
#endif

  [[nodiscard]]
  bool                  managed            () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Request           native             () const
  {
    return native_;
  }
  [[nodiscard]]
  bool                  persistent         () const
  {
    return persistent_;
  }

protected:
  friend class communicator;
  friend class message;
  friend class window;
  friend class io::file;
  friend std::optional<std::vector<status>>               test_all (std::vector<request>& requests);
  friend std::optional<std::tuple <std::int32_t, status>> test_any (std::vector<request>& requests);
  friend std::vector  <std::tuple <std::int32_t, status>> test_some(std::vector<request>& requests);
  friend std::vector  <status>                            wait_all (std::vector<request>& requests);
  friend std::tuple   <std::int32_t, status>              wait_any (std::vector<request>& requests);
  friend std::vector  <std::tuple<std::int32_t, status>>  wait_some(std::vector<request>& requests);

  bool        managed_    = false;
  MPI_Request native_     = MPI_REQUEST_NULL;
  bool        persistent_ = false;
};

inline std::optional<std::vector<status>>               test_all (std::vector<request>& requests)
{
  std::vector<MPI_Request> raw_requests(requests.size());
  std::int32_t             complete    (0);
  std::vector<status>      result      (requests.size());

  std::ranges::transform(requests, raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_CHECK_ERROR_CODE(MPI_Testall, (static_cast<std::int32_t>(requests.size()), raw_requests.data(), &complete, result.data()))

  for (std::size_t i = 0; i < requests.size(); ++i)
    requests[i].native_ = raw_requests[i];

  return static_cast<bool>(complete) ? result : std::optional<std::vector<status>>(std::nullopt);
}
inline std::optional<std::tuple <std::int32_t, status>> test_any (std::vector<request>& requests)
{
  std::vector<MPI_Request>         raw_requests(requests.size());
  std::int32_t                     complete    (0);
  std::tuple<std::int32_t, status> result;

  std::ranges::transform(requests, raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_CHECK_ERROR_CODE(MPI_Testany, (static_cast<std::int32_t>(requests.size()), raw_requests.data(), &std::get<0>(result), &complete, &std::get<1>(result)))
  // MPI_CHECK_UNDEFINED (MPI_Testany, std::get<0>(result)) // MPI_UNDEFINED should not cause an exception in this case.

  for (std::size_t i = 0; i < requests.size(); ++i)
    requests[i].native_ = raw_requests[i];

  return static_cast<bool>(complete) ? result : std::optional<std::tuple<std::int32_t, status>>(std::nullopt);
}
inline std::vector  <std::tuple <std::int32_t, status>> test_some(std::vector<request>& requests)
{
  std::vector<MPI_Request>  raw_requests(requests.size());
  std::int32_t              count       (0);
  std::vector<std::int32_t> indices     (requests.size());
  std::vector<status>       stati       (requests.size());

  std::ranges::transform(requests, raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_CHECK_ERROR_CODE(MPI_Testsome, (static_cast<std::int32_t>(requests.size()), raw_requests.data(), &count, indices.data(), stati.data()))
  // MPI_CHECK_UNDEFINED (MPI_Testsome, count) // MPI_UNDEFINED should not cause an exception in this case.

  for (std::size_t i = 0; i < requests.size(); ++i)
    requests[i].native_ = raw_requests[i];

  if (count == MPI_UNDEFINED)
    return {};

  indices.resize(static_cast<std::size_t>(count));
  stati  .resize(static_cast<std::size_t>(count));

  std::vector<std::tuple<std::int32_t, status>> result(count);
  std::transform(indices.begin(), indices.end(), stati.begin(), result.begin(), [ ] (const auto& lhs, const auto& rhs)
  {
    return std::tuple<std::int32_t, status>{lhs, rhs};
  });
  return result;
}

inline std::vector<status>                              wait_all (std::vector<request>& requests)
{
  std::vector<MPI_Request> raw_requests(requests.size());
  std::vector<status>      result      (requests.size());

  std::ranges::transform(requests, raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_CHECK_ERROR_CODE(MPI_Waitall, (static_cast<std::int32_t>(requests.size()), raw_requests.data(), result.data()))

  for (std::size_t i = 0; i < requests.size(); ++i)
    requests[i].native_ = raw_requests[i];

  return result;
}
inline std::tuple <std::int32_t, status>                wait_any (std::vector<request>& requests)
{
  std::vector<MPI_Request>         raw_requests(requests.size());
  std::tuple<std::int32_t, status> result;

  std::ranges::transform(requests, raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_CHECK_ERROR_CODE(MPI_Waitany, (static_cast<std::int32_t>(requests.size()), raw_requests.data(), &std::get<0>(result), &std::get<1>(result)))
  // MPI_CHECK_UNDEFINED (MPI_Waitany, std::get<0>(result)) // MPI_UNDEFINED should not cause an exception in this case.

  for (std::size_t i = 0; i < requests.size(); ++i)
    requests[i].native_ = raw_requests[i];

  return result;
}
inline std::vector<std::tuple<std::int32_t, status>>    wait_some(std::vector<request>& requests)
{
  std::vector<MPI_Request>  raw_requests(requests.size());
  std::int32_t              count       (0);
  std::vector<std::int32_t> indices     (requests.size());
  std::vector<status>       stati       (requests.size());

  std::ranges::transform(requests, raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_CHECK_ERROR_CODE(MPI_Waitsome, (static_cast<std::int32_t>(requests.size()), raw_requests.data(), &count, indices.data(), stati.data()))
  // MPI_CHECK_UNDEFINED (MPI_Waitsome, count) // MPI_UNDEFINED should not cause an exception in this case.

  for (std::size_t i = 0; i < requests.size(); ++i)
    requests[i].native_ = raw_requests[i];

  if (count == MPI_UNDEFINED)
    return {};

  indices.resize(static_cast<std::size_t>(count));
  stati  .resize(static_cast<std::size_t>(count));

  std::vector<std::tuple<std::int32_t, status>> result(count);
  std::transform(indices.begin(), indices.end(), stati.begin(), result.begin(), [ ] (const auto& lhs, const auto& rhs)
  {
    return std::tuple<std::int32_t, status>{lhs, rhs};
  });
  return result;
}

inline void                                             start_all(const std::vector<request>& requests)
{
  std::vector<MPI_Request> raw_requests(requests.size());

  std::ranges::transform(requests, raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_CHECK_ERROR_CODE(MPI_Startall, (static_cast<std::int32_t>(raw_requests.size()), raw_requests.data()))
}
}