#pragma once

#include <cstdint>
#include <functional>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/request.hpp>
#include <mpi/core/status.hpp>

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
    void*                      extra_state    )
  : request(MPI_REQUEST_NULL, true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Grequest_start, (query_function, free_function, cancel_function, extra_state, &native_))
  }
  explicit generalized_request  (
    const std::function<error_code(status)>& query_function , 
    const std::function<error_code()>&       free_function  ,
    const std::function<error_code(bool)>&   cancel_function)
  : request(MPI_REQUEST_NULL, true), query_function_(query_function), free_function_(free_function), cancel_function_(cancel_function)
  {
    MPI_CHECK_ERROR_CODE(MPI_Grequest_start, (
      [ ] (void* this_pointer, MPI_Status* status)
      {
        return static_cast<generalized_request*>(this_pointer)->query_function_ (*status)                    .error_class().native();
      }, 
      [ ] (void* this_pointer)
      {
        return static_cast<generalized_request*>(this_pointer)->free_function_  ()                           .error_class().native();
      },
      [ ] (void* this_pointer, const std::int32_t complete)
      {
        return static_cast<generalized_request*>(this_pointer)->cancel_function_(static_cast<bool>(complete)).error_class().native();
      }, this, &native_))
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

protected:
  std::function<error_code(status)> query_function_ ;
  std::function<error_code()>       free_function_  ;
  std::function<error_code(bool)>   cancel_function_;
};
}