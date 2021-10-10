#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/control_variable_handle.hpp>
#include <mpi/tool/performance_variable_handle.hpp>

namespace mpi::tool
{
struct category
{
  explicit category  (std::int32_t index);
  category           (const category&  that) = default;
  category           (      category&& temp) = default;
  virtual ~category  ()                      = default;
  category& operator=(const category&  that) = default;
  category& operator=(      category&& temp) = default;

  std::int32_t                      index                ;
  std::string                       name                 ;
  std::string                       description          ;
  std::vector<control_variable>     control_variables    ;
  std::vector<performance_variable> performance_variables;
  std::vector<category>             subcategories        ;
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
inline std::vector<category> categories        (const std::vector<std::int32_t>& indices)
{
  const auto count = static_cast<std::int32_t>(indices.size());

  std::vector<category> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(indices[i]);

  return result;
}

inline category::category(const std::int32_t index) : index(index)
{
  std::int32_t name_length, description_length, control_variable_count, performance_variable_count, subcategory_count;

  MPI_CHECK_ERROR_CODE(MPI_T_category_get_info, (
    index                       ,
    nullptr                     ,
    &name_length                ,
    nullptr                     ,
    &description_length         ,
    &control_variable_count     ,
    &performance_variable_count ,
    &subcategory_count          ))

  name       .resize(name_length       );
  description.resize(description_length);

  MPI_CHECK_ERROR_CODE(MPI_T_category_get_info, (
    index                       ,
    name       .data()          ,
    &name_length                ,
    description.data()          ,
    &description_length         ,
    &control_variable_count     ,
    &performance_variable_count ,
    &subcategory_count          ))

  std::vector<std::int32_t> control_variable_indices    (control_variable_count    ),
                            performance_variable_indices(performance_variable_count),
                            subcategory_indices         (subcategory_count         );

  MPI_CHECK_ERROR_CODE(MPI_T_category_get_cvars     , (index, control_variable_count    , control_variable_indices    .data()))
  MPI_CHECK_ERROR_CODE(MPI_T_category_get_pvars     , (index, performance_variable_count, performance_variable_indices.data()))
  MPI_CHECK_ERROR_CODE(MPI_T_category_get_categories, (index, subcategory_count         , subcategory_indices         .data()))

  control_variables     = tool::control_variables    (control_variable_indices    );
  performance_variables = tool::performance_variables(performance_variable_indices);
  subcategories         = tool::categories           (subcategory_indices         );
}
}