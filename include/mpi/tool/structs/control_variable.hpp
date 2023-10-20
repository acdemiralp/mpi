#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

#include <mpi/core/type/data_type.hpp>
#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/scope.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/enumeration.hpp>

namespace mpi::tool
{
struct control_variable
{
  explicit control_variable  (const std::int32_t index) : index(index)
  {
    auto         name_size(0), description_size(0);
    MPI_Datatype raw_data_type(MPI_DATATYPE_NULL);
    MPI_T_enum   enum_type    (MPI_T_ENUM_NULL  );

    MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_info, (
      index                                      ,
      nullptr                                    , 
      &name_size                                 , 
      reinterpret_cast<std::int32_t*>(&verbosity),
      &raw_data_type                             ,
      &enum_type                                 ,
      nullptr                                    ,
      &description_size                          ,
      reinterpret_cast<std::int32_t*>(&bind_type),
      reinterpret_cast<std::int32_t*>(&scope)    ))

    name       .resize(name_size);
    description.resize(description_size);

    MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_info, (
      index                                      ,
      name.data()                                ,
      &name_size                                 , 
      reinterpret_cast<std::int32_t*>(&verbosity),
      &raw_data_type                             ,
      &enum_type                                 ,
      description.data()                         ,
      &description_size                          ,
      reinterpret_cast<std::int32_t*>(&bind_type),
      reinterpret_cast<std::int32_t*>(&scope)    ))

    data_type.emplace(raw_data_type, true);
    if (enum_type != MPI_T_ENUM_NULL)
      enumeration.emplace(enum_type);
  }
  explicit control_variable  (const std::string& name);
  control_variable           (const control_variable&  that) = delete ;
  control_variable           (      control_variable&& temp) = default;
  virtual ~control_variable  ()                              = default;
  control_variable& operator=(const control_variable&  that) = delete ;
  control_variable& operator=(      control_variable&& temp) = default;

  std::int32_t               index      ;

  std::string                name       ;
  std::string                description;

  mpi::tool::bind_type       bind_type  ;
  mpi::tool::scope           scope      ;
  mpi::tool::verbosity       verbosity  ;

  std::optional<data_type>   data_type  ; // Abusing optional for delayed construction of a stack variable.
  std::optional<enumeration> enumeration;
};

inline std::int32_t                  control_variable_index(const std::string& name)
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_index, (name.c_str(), &result))
  return result;
}
inline std::int32_t                  control_variable_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_num, (&result))
  return result;
}
inline std::vector<control_variable> control_variables     ()
{
  const auto count = control_variable_count();

  std::vector<control_variable> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(i);

  return result;
}
inline std::vector<control_variable> control_variables     (const std::vector<std::int32_t>& indices)
{
  const auto count = static_cast<std::int32_t>(indices.size());

  std::vector<control_variable> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(indices[i]);

  return result;
}

inline control_variable::control_variable(const std::string& name)
: control_variable(control_variable_index(name))
{
  
}
}