#pragma once

#include <cstdint>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/request.hpp>

namespace mpi
{
class generalized_request : public request
{
public:
  using query_function_type  = std::int32_t (*) (void*, MPI_Status*);
  using free_function_type   = std::int32_t (*) (void*);
  using cancel_function_type = std::int32_t (*) (void*, std::int32_t);

  explicit generalized_request  (
    const query_function_type  query_function , 
    const free_function_type   free_function  , 
    const cancel_function_type cancel_function, 
    void*                      extra_state    = nullptr)
  {
    managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Grequest_start, (query_function, free_function, cancel_function, extra_state, &native_))
  }
  explicit generalized_request  (const MPI_Request native)
  : request(native)
  {
    
  }
  generalized_request           (const generalized_request&    that)          = delete ;
  generalized_request           (      generalized_request&&   temp) noexcept = default;
 ~generalized_request           ()                                   override = default;
  generalized_request& operator=(const generalized_request&    that)          = delete ;
  generalized_request& operator=(      generalized_request&&   temp) noexcept = default;

  void complete() const
  {
    MPI_CHECK_ERROR_CODE(MPI_Grequest_complete, (native_))
  }
};
}