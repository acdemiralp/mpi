#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/control_variable.hpp>
#include <mpi/tool/performance_variable.hpp>

namespace mpi::tool
{
struct category
{
  explicit category  (const std::int32_t index)
  {
    std::int32_t name_length, description_length, control_variable_count, performance_variable_count, subcategory_count;
-
    MPI_CHECK_ERROR_CODE(MPI_T_category_get_info, (
      index                       ,
      nullptr                     ,
      &name_length                ,
      nullptr                     ,
      &description_length         ,
      &control_variable_count     ,
      &performance_variable_count ,
      &subcategory_count          ))

    name                 .resize(name_length               );
    description          .resize(description_length        );
    control_variables    .resize(control_variable_count    );
    performance_variables.resize(performance_variable_count);
    subcategories        .resize(subcategory_count         );

    MPI_CHECK_ERROR_CODE(MPI_T_category_get_info, (
      index                       ,
      name       .data()          ,
      &name_length                ,
      description.data()          ,
      &description_length         ,
      &control_variable_count     ,
      &performance_variable_count ,
      &subcategory_count          ))

    std::vector<std::int32_t> control_variables    (control_variable_count    );
    std::vector<std::int32_t> performance_variables(performance_variable_count);
    std::vector<std::int32_t> subcategories        (subcategory_count         );

    MPI_CHECK_ERROR_CODE(MPI_T_category_get_cvars     , (index, control_variable_count    , control_variables    .data()))
    MPI_CHECK_ERROR_CODE(MPI_T_category_get_pvars     , (index, performance_variable_count, performance_variables.data()))
    MPI_CHECK_ERROR_CODE(MPI_T_category_get_categories, (index, subcategory_count         , subcategories        .data()))

    for (std::size_t j = 0; j < control_variables    .size(); ++j)
    {
      auto& control_variable     = control_variables    [j];

      // TODO
    }
    for (std::size_t j = 0; j < performance_variables.size(); ++j)
    {
      auto& performance_variable = performance_variables[j];

      // TODO
    }
    for (std::size_t j = 0; j < subcategories        .size(); ++j)
    {
      auto& subcategory          = subcategories        [j];

      // TODO
    }
  }
  category           (const category&  that) = delete ;
  category           (      category&& temp) = default;
  virtual ~category  ()                      = default;
  category& operator=(const category&  that) = delete ;
  category& operator=(      category&& temp) = default;

  std::string                                   name                 ;
  std::string                                   description          ;
  std::vector<control_variable_information>     control_variables    ;
  std::vector<performance_variable_information> performance_variables;
  std::vector<category>                         subcategories        ;
};

inline bool                  categories_changed()
{
  static auto last_timestamp(0);
         auto timestamp     (0);

  MPI_CHECK_ERROR_CODE(MPI_T_category_changed, (&timestamp))

  if (last_timestamp == timestamp)
    return false;

  last_timestamp = timestamp;
  return true;
}
inline std::int32_t          category_count    ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_category_get_num, (&result))
  return result;
}
inline std::vector<category> categories        ()
{
  const auto count = category_count();

  std::vector<category> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(i);

  return result;
}
}