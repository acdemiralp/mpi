#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

#include <mpi/core/type/data_type.hpp>
#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/performance_variable_type.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/enumeration.hpp>

namespace mpi::tool
{
struct performance_variable
{
  explicit performance_variable  (const std::int32_t index) : index(index)
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

    data_type.emplace(raw_data_type);
    if (enum_type != MPI_T_ENUM_NULL)
      enumeration.emplace(enum_type);
  }
  explicit performance_variable  (const std::string& name , performance_variable_type type);
  performance_variable           (const performance_variable&  that) = delete ;
  performance_variable           (      performance_variable&& temp) = default;
  virtual ~performance_variable  ()                                  = default;
  performance_variable& operator=(const performance_variable&  that) = delete ;
  performance_variable& operator=(      performance_variable&& temp) = default;

  std::int32_t               index      ;

  std::string                name       ;
  std::string                description;

  bind_type                  bind_type  ;
  performance_variable_type  type       ;
  verbosity                  verbosity  ;
  bool                       read_only  ;
  bool                       continuous ;
  bool                       atomic     ;

  std::optional<data_type>   data_type  ; // Abusing optional for delayed construction of a stack variable.
  std::optional<enumeration> enumeration;
};

inline std::int32_t                      performance_variable_index(const std::string& name, const performance_variable_type type)
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_index(name.c_str(), static_cast<std::int32_t>(type), &result))
  return result;
}
inline std::int32_t                      performance_variable_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_num, (&result))
  return result;
}
inline std::vector<performance_variable> performance_variables     ()
{
  const auto count = performance_variable_count();

  std::vector<performance_variable> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(i);

  return result;
}
inline std::vector<performance_variable> performance_variables     (const std::vector<std::int32_t>& indices)
{
  const auto count = static_cast<std::int32_t>(indices.size());

  std::vector<performance_variable> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(indices[i]);

  return result;
}

inline performance_variable::performance_variable(const std::string& name , const performance_variable_type type)
: performance_variable(performance_variable_index(name, type))
{
  
}
}