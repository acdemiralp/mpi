#pragma once

#include <string>
#include <optional>

#include <mpi/core/data_type.hpp>
#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/performance_variable_type.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/enum_information.hpp>

namespace mpi::tool
{
struct performance_variable_information
{
  explicit performance_variable_information  (const std::int32_t index)
  {
    auto         name_size(0), description_size(0);
    MPI_Datatype raw_data_type;
    MPI_T_enum   enum_type;

    MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_info, (
      index                                       ,
      nullptr                                     , 
      &name_size                                  , 
      reinterpret_cast<std::int32_t*>(&verbosity) ,
      reinterpret_cast<std::int32_t*>(&type)      ,
      &raw_data_type                              ,
      &enum_type                                  ,
      nullptr                                     ,
      &description_size                           ,
      reinterpret_cast<std::int32_t*>(&bind_type) ,
      reinterpret_cast<std::int32_t*>(&read_only) ,
      reinterpret_cast<std::int32_t*>(&continuous),
      reinterpret_cast<std::int32_t*>(&atomic)    ))

    name       .resize(name_size);
    description.resize(description_size);

    MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_info, (
      index                                       ,
      name.data()                                 , 
      &name_size                                  , 
      reinterpret_cast<std::int32_t*>(&verbosity) ,
      reinterpret_cast<std::int32_t*>(&type)      ,
      &raw_data_type                              ,
      &enum_type                                  ,
      description.data()                          ,
      &description_size                           ,
      reinterpret_cast<std::int32_t*>(&bind_type) ,
      reinterpret_cast<std::int32_t*>(&read_only) ,
      reinterpret_cast<std::int32_t*>(&continuous),
      reinterpret_cast<std::int32_t*>(&atomic)    ))

    data_type = mpi::data_type(raw_data_type);
    if (enum_type != MPI_T_ENUM_NULL)
      enum_information = tool::enum_information(enum_type);
  }
  performance_variable_information           (const performance_variable_information&  that) = delete ;
  performance_variable_information           (      performance_variable_information&& temp) = default;
  virtual ~performance_variable_information  ()                                              = default;
  performance_variable_information& operator=(const performance_variable_information&  that) = delete ;
  performance_variable_information& operator=(      performance_variable_information&& temp) = default;

  std::string                     name            ;
  std::string                     description     ;

  bind_type                       bind_type       ;
  performance_variable_type       type            ;
  verbosity                       verbosity       ;
  bool                            read_only       ;
  bool                            continuous      ;
  bool                            atomic          ;

  data_type                       data_type       ;
  std::optional<enum_information> enum_information;
};
}