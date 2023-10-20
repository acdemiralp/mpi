#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <vector>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/enumeration.hpp>

namespace mpi::tool
{
#ifdef MPI_GEQ_4_0
struct event
{
  explicit event  (const std::int32_t index) : index(index)
  {
    auto                      name_size(0), description_size(0), size(0);
    MPI_Info                  raw_information(MPI_INFO_NULL  );
    MPI_T_enum                enum_type      (MPI_T_ENUM_NULL);
    std::vector<MPI_Datatype> raw_data_types;

    MPI_CHECK_ERROR_CODE(MPI_T_event_get_info, (
      index                                      ,
      nullptr                                    , 
      &name_size                                 , 
      reinterpret_cast<std::int32_t*>(&verbosity),
      nullptr                                    , 
      nullptr                                    , 
      &size                                      ,
      &enum_type                                 ,
      &raw_information                           ,
      nullptr                                    , 
      &description_size                          ,
      reinterpret_cast<std::int32_t*>(&bind_type)))

    name          .resize (name_size);
    description   .resize (description_size);
    data_types    .reserve(size);
    displacements .resize (size);
    raw_data_types.resize (size);

    MPI_CHECK_ERROR_CODE(MPI_T_event_get_info, (
      index                                      ,
      name          .data()                      , 
      &name_size                                 , 
      reinterpret_cast<std::int32_t*>(&verbosity),
      raw_data_types.data()                      ,
      displacements .data()                      ,
      &size                                      ,
      &enum_type                                 ,
      &raw_information                           ,
      description   .data()                      ,
      &description_size                          ,
      reinterpret_cast<std::int32_t*>(&bind_type)))

    for (auto& type : raw_data_types)
      data_types.emplace_back(type, true);
    if (raw_information != MPI_INFO_NULL  )
      information.emplace    (raw_information, true);
    if (enum_type       != MPI_T_ENUM_NULL)
      enumeration.emplace    (enum_type);
  }
  event           (const event&  that) = delete ;
  event           (      event&& temp) = default;
  virtual ~event  ()                   = default;
  event& operator=(const event&  that) = delete ;
  event& operator=(      event&& temp) = default;

  std::int32_t                          index        ;

  std::string                           name         ;
  std::string                           description  ;

  mpi::tool::bind_type                  bind_type    ;
  mpi::tool::verbosity                  verbosity    ;

  std::vector<data_type>                data_types   ;
  std::vector<aint>                     displacements;

  std::optional<mpi::information>       information  ;
  std::optional<mpi::tool::enumeration> enumeration  ;
};

inline std::int32_t       event_index(const std::string& name)
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_event_get_index, (name.c_str(), &result))
  return result;
}
inline std::int32_t       event_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_event_get_num, (&result))
  return result;
}
inline std::vector<event> events     ()
{
  const auto count = event_count();

  std::vector<event> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(i);

  return result;
}
inline std::vector<event> events     (const std::vector<std::int32_t>& indices)
{
  const auto count = static_cast<std::int32_t>(indices.size());

  std::vector<event> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(indices[i]);

  return result;
}
#endif
}
