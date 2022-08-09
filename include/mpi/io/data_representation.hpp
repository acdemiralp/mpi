#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::io
{
class data_representation
{
public:
  using conversion_function_type = std::int32_t (*) (void*, MPI_Datatype, std::int32_t, void*, offset, void*);
  using extent_function_type     = std::int32_t (*) (       MPI_Datatype, aint*                      , void*);

  data_representation           (
    std::string                    name           ,
    const conversion_function_type read_function  ,
    const conversion_function_type write_function ,
    const extent_function_type     extent_function,
    void*                          user_data      )
  : name_(std::move(name))
  {
    MPI_CHECK_ERROR_CODE(MPI_Register_datarep, (name.c_str(), read_function, write_function, extent_function, user_data))
  }
  data_representation           (
    std::string                                                                       name           ,
    std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, offset)> read_function  ,
    std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, offset)> write_function ,
    std::function<std::int32_t(const data_type&, aint*)>                              extent_function)
  : name_(std::move(name)), read_function_(std::move(read_function)), write_function_(std::move(write_function)), extent_function_(std::move(extent_function))
  {
    MPI_CHECK_ERROR_CODE(MPI_Register_datarep, (
      name.c_str(), 
      [ ] (void* buffer, MPI_Datatype data_type, std::int32_t count  , void* file_buffer, offset position, void* extra_state)
      {
        return static_cast<data_representation*>(extra_state)->read_function_ (buffer, mpi::data_type(data_type), count, file_buffer, position);
      }, 
      [ ] (void* buffer, MPI_Datatype data_type, std::int32_t count  , void* file_buffer, offset position, void* extra_state)
      {
        return static_cast<data_representation*>(extra_state)->write_function_(buffer, mpi::data_type(data_type), count, file_buffer, position);
      }, 
      [ ] (              MPI_Datatype data_type, aint*        extent  ,                                    void* extra_state)
      {
        return static_cast<data_representation*>(extra_state)->extent_function_(       mpi::data_type(data_type), extent);
      },
      this))
  }
  explicit data_representation  (std::string name = "external32")
  : name_(std::move(name))
  {

  }
  data_representation           (const data_representation&  that) = default;
  data_representation           (      data_representation&& temp) = default;
  virtual ~data_representation  ()                                 = default;
  data_representation& operator=(const data_representation&  that) = default;
  data_representation& operator=(      data_representation&& temp) = default;

  [[nodiscard]]
  aint               pack_size(const std::int32_t size, const data_type& data_type) const
  {
    aint result;
    MPI_CHECK_ERROR_CODE(MPI_Pack_external_size, (name_.c_str(), size, data_type.native(), &result))
    return result;
  }
  
  [[nodiscard]]
  aint               pack     (const void*       input , const std::int32_t input_size , const data_type& input_data_type ,
                                     void*       output, const aint         output_size, const aint       output_position = 0) const
  {
    aint result(output_position);
    MPI_CHECK_ERROR_CODE(MPI_Pack_external     , (name_.c_str(), input, input_size, input_data_type.native(), output, output_size, &result ))
    return result;
  }
  template <typename input_type, typename output_type> [[nodiscard]]
  aint               pack     (const input_type& input , output_type&       output     , const aint       output_position = 0, const bool resize = false) const
  {
    using input_adapter  = container_adapter<input_type >;
    using output_adapter = container_adapter<output_type>;

    if (resize)
      output_adapter::resize(output, pack_size(input_adapter::size(input), input_adapter::data_type()) / sizeof(typename output_adapter::value_type));

    return pack(input_adapter::data(input), static_cast<std::int32_t>(input_adapter::size(input)), input_adapter::data_type(), output_adapter::data(output), static_cast<aint>(output_adapter::size(output)), output_position);
  }
  
  [[nodiscard]]
  aint               unpack   (const void*       input , const aint         input_size , const aint       input_position  ,
                                     void*       output, const std::int32_t output_size, const data_type& output_data_type) const
  {
    aint result(input_position);
    MPI_CHECK_ERROR_CODE(MPI_Unpack_external   , (name_.c_str(), input, input_size, &result, output, output_size, output_data_type.native()))
    return result;
  }
  template <typename input_type, typename output_type> [[nodiscard]]
  aint               unpack   (const input_type& input , output_type&       output     , const aint       input_position  = 0) const
  {
    using input_adapter  = container_adapter<input_type >;
    using output_adapter = container_adapter<output_type>;

    return unpack(input_adapter::data(input), static_cast<aint>(input_adapter::size(input)), input_position, output_adapter::data(output), static_cast<std::int32_t>(output_adapter::size(output)), output_adapter::data_type());
  }

  [[nodiscard]]
  const std::string& name     () const
  {
    return name_;
  }
  
protected:
  std::string                                                                       name_           ;
  std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, offset)> read_function_  ;
  std::function<std::int32_t(void*, const data_type&, std::int32_t, void*, offset)> write_function_ ;
  std::function<std::int32_t(       const data_type&, aint*                      )> extent_function_;
};
}