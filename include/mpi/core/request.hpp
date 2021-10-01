#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <tuple>
#include <vector>

#include <mpi/core/mpi.hpp>
#include <mpi/core/status.hpp>

namespace mpi
{
class request
{
public:
  request           ()                     = default;
  request           (const request&  that) = delete;
  request           (      request&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_REQUEST_NULL;
  }
  virtual ~request  ()
  {
    if (managed_ && native_ != MPI_REQUEST_NULL)
      MPI_Request_free(&native_);
  }
  request& operator=(const request&  that) = delete;
  request& operator=(      request&& temp) noexcept
  {
    if (this != &temp)
    {
      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_REQUEST_NULL;
    }
    return *this;
  }
  
  std::optional<status> get_status()
  {
    std::int32_t complete;
    status       result  ;
    MPI_Request_get_status(native_, &complete, &result);
    return static_cast<bool>(complete) ? result : std::optional<status>(std::nullopt);
  }
  std::optional<status> test      ()
  {
    std::int32_t complete;
    status       result  ;
    MPI_Test(&native_, &complete, &result);
    return static_cast<bool>(complete) ? result : std::optional<status>(std::nullopt);
  }
  status                wait      ()
  {
    status result;
    MPI_Wait(&native_, &result);
    return result;
  }
  bool                  cancel    ()
  {
    return MPI_Cancel(&native_) == MPI_SUCCESS;
  }

  [[nodiscard]]
  bool                  managed   () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Request           native    () const
  {
    return native_;
  }

protected:
  bool        managed_ = false;
  MPI_Request native_  = MPI_REQUEST_NULL;
};

inline std::optional<std::vector<status>>               test_all (const std::vector<request>& requests)
{
  std::vector<MPI_Request> raw_requests(requests.size());
  std::int32_t             complete    (0);
  std::vector<status>      result      (requests.size());

  std::transform(requests.begin(), requests.end(), raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_Testall(static_cast<std::int32_t>(requests.size()), raw_requests.data(), &complete, result.data());

  return static_cast<bool>(complete) ? result : std::optional<std::vector<status>>(std::nullopt);
}
inline std::optional<std::tuple <std::int32_t, status>> test_any (const std::vector<request>& requests)
{
  std::vector<MPI_Request>         raw_requests(requests.size());
  std::int32_t                     complete    (0);
  std::tuple<std::int32_t, status> result;

  std::transform(requests.begin(), requests.end(), raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_Testany(static_cast<std::int32_t>(requests.size()), raw_requests.data(), &std::get<0>(result), &complete, &std::get<1>(result));

  return static_cast<bool>(complete) ? result : std::optional<std::tuple<std::int32_t, status>>(std::nullopt);
}
inline std::vector  <std::tuple <std::int32_t, status>> test_some(const std::vector<request>& requests)
{
  std::vector<MPI_Request>  raw_requests(requests.size());
  std::int32_t              count       (0);
  std::vector<std::int32_t> indices     (requests.size());
  std::vector<status>       stati       (requests.size());

  std::transform(requests.begin(), requests.end(), raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_Testsome(static_cast<std::int32_t>(requests.size()), raw_requests.data(), &count, indices.data(), stati.data());

  if (count == MPI_UNDEFINED)
    return std::vector<std::tuple<std::int32_t, status>>();

  indices.resize(count);
  stati  .resize(count);

  std::vector<std::tuple<std::int32_t, status>> result(count);
  std::transform(indices.begin(), indices.end(), stati.begin(), result.begin(), [ ] (const auto& lhs, const auto& rhs)
  {
    return std::tuple<std::int32_t, status>{lhs, rhs};
  });
  return result;
}

inline std::vector<status>                              wait_all (const std::vector<request>& requests)
{
  std::vector<MPI_Request> raw_requests(requests.size());
  std::vector<status>      result      (requests.size());

  std::transform(requests.begin(), requests.end(), raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_Waitall(static_cast<std::int32_t>(requests.size()), raw_requests.data(), result.data());

  return result;
}
inline std::tuple <std::int32_t, status>                wait_any (const std::vector<request>& requests)
{
  std::vector<MPI_Request>         raw_requests(requests.size());
  std::tuple<std::int32_t, status> result;

  std::transform(requests.begin(), requests.end(), raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_Waitany(static_cast<std::int32_t>(requests.size()), raw_requests.data(), &std::get<0>(result), &std::get<1>(result));

  return result;
}
inline std::vector<std::tuple<std::int32_t, status>>    wait_some(const std::vector<request>& requests)
{
  std::vector<MPI_Request>  raw_requests(requests.size());
  std::int32_t              count       (0);
  std::vector<std::int32_t> indices     (requests.size());
  std::vector<status>       stati       (requests.size());

  std::transform(requests.begin(), requests.end(), raw_requests.begin(), [ ] (const auto& request)
  {
    return request.native();
  });

  MPI_Waitsome(static_cast<std::int32_t>(requests.size()), raw_requests.data(), &count, indices.data(), stati.data());

  if (count == MPI_UNDEFINED)
    return std::vector<std::tuple<std::int32_t, status>>();

  indices.resize(count);
  stati  .resize(count);

  std::vector<std::tuple<std::int32_t, status>> result(count);
  std::transform(indices.begin(), indices.end(), stati.begin(), result.begin(), [ ] (const auto& lhs, const auto& rhs)
  {
    return std::tuple<std::int32_t, status>{lhs, rhs};
  });
  return result;
}
}