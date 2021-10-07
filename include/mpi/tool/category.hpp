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
  std::vector<category> result(category_count());
  
  for (std::size_t i = 0; i < result.size(); ++i)
  {
    auto& category = result[i];

    std::int32_t name_length, description_length, control_variable_count, performance_variable_count, subcategory_count;
-
    MPI_CHECK_ERROR_CODE(MPI_T_category_get_info, (
      static_cast<std::int32_t>(i),
      nullptr                     ,
      &name_length                ,
      nullptr                     ,
      &description_length         ,
      &control_variable_count     ,
      &performance_variable_count ,
      &subcategory_count          ))

    category.name                 .resize(static_cast<std::size_t>(name_length               ));
    category.description          .resize(static_cast<std::size_t>(description_length        ));
    category.control_variables    .resize(static_cast<std::size_t>(control_variable_count    ));
    category.performance_variables.resize(static_cast<std::size_t>(performance_variable_count));
    category.subcategories        .resize(static_cast<std::size_t>(subcategory_count         ));

    MPI_CHECK_ERROR_CODE(MPI_T_category_get_info, (
      static_cast<std::int32_t>(i),
      category.name       .data() ,
      &name_length                ,
      category.description.data() ,
      &description_length         ,
      &control_variable_count     ,
      &performance_variable_count ,
      &subcategory_count         ))

    std::vector<std::int32_t> control_variables    (control_variable_count    );
    std::vector<std::int32_t> performance_variables(performance_variable_count);
    std::vector<std::int32_t> subcategories        (subcategory_count         );

    MPI_CHECK_ERROR_CODE(MPI_T_category_get_cvars     , (static_cast<std::int32_t>(i), control_variable_count    , control_variables    .data()))
    MPI_CHECK_ERROR_CODE(MPI_T_category_get_pvars     , (static_cast<std::int32_t>(i), performance_variable_count, performance_variables.data()))
    MPI_CHECK_ERROR_CODE(MPI_T_category_get_categories, (static_cast<std::int32_t>(i), subcategory_count         , subcategories        .data()))

    for (std::size_t j = 0; j < category.control_variables    .size(); ++j)
    {
      auto& control_variable     = category.control_variables    [j];

      // TODO
    }
    for (std::size_t j = 0; j < category.performance_variables.size(); ++j)
    {
      auto& performance_variable = category.performance_variables[j];

      // TODO
    }
    for (std::size_t j = 0; j < category.subcategories        .size(); ++j)
    {
      auto& subcategory          = category.subcategories        [j];

      // TODO
    }
  }

  return result;
}
}