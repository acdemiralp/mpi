#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::io
{
class data_representation
{
public:
  using conversion_function_type = std::int32_t (*) (void*, MPI_Datatype, std::int32_t, void*, std::int64_t, void*);
  using extent_function_type     = std::int32_t (*) (       MPI_Datatype, std::int64_t*                    , void*);

  data_representation           (
    const std::string&             name           ,
    const conversion_function_type read_function  ,
    const conversion_function_type write_function ,
    const extent_function_type     extent_function,
    void*                          user_data      )
  : name_(name)
  {
    MPI_CHECK_ERROR_CODE(MPI_Register_datarep, (name.c_str(), read_function, write_function, extent_function, user_data))
  }
  data_representation           (
    const std::string&                                                                             name           ,
    const std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, std::int64_t)>& read_function  ,
    const std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, std::int64_t)>& write_function ,
    const std::function<std::int32_t(       const data_type&, std::int64_t*                    )>& extent_function)
  : name_(name), read_function_(read_function), write_function_(write_function), extent_function_(extent_function)
  {
    MPI_CHECK_ERROR_CODE(MPI_Register_datarep, (
      name.c_str(), 
      [ ] (void* buffer, MPI_Datatype data_type, std::int32_t count  , void* file_buffer, std::int64_t position, void* extra_state)
      {
        return static_cast<data_representation*>(extra_state)->read_function_ (buffer, mpi::data_type(data_type), count, file_buffer, position);
      }, 
      [ ] (void* buffer, MPI_Datatype data_type, std::int32_t count  , void* file_buffer, std::int64_t position, void* extra_state)
      {
        return static_cast<data_representation*>(extra_state)->write_function_(buffer, mpi::data_type(data_type), count, file_buffer, position);
      }, 
      [ ] (              MPI_Datatype data_type, std::int64_t* extent                                          , void* extra_state)
      {
        return static_cast<data_representation*>(extra_state)->extent_function_(       mpi::data_type(data_type), extent);
      },
      this))
  }
  data_representation           (const data_representation&  that) = default;
  data_representation           (      data_representation&& temp) = default;
  virtual ~data_representation  ()                                 = default;
  data_representation& operator=(const data_representation&  that) = default;
  data_representation& operator=(      data_representation&& temp) = default;

  [[nodiscard]]
  const std::string& name() const
  {
    return name_;
  }
  
protected:
  std::string                                                                             name_           ;
  std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, std::int64_t)> read_function_  ;
  std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, std::int64_t)> write_function_ ;
  std::function<std::int32_t(       const data_type&, std::int64_t*                    )> extent_function_;

};
}