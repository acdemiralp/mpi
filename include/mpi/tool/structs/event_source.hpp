#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
#ifdef MPI_GEQ_4_0
struct event_source
{
  explicit event_source  (const std::int32_t index) : index(index)
  {
    auto               name_size(0), description_size(0);
    MPI_Info           raw_information(MPI_INFO_NULL);
    MPI_T_source_order order;

    MPI_CHECK_ERROR_CODE(MPI_T_source_get_info, (
      index            ,
      nullptr          , 
      &name_size       , 
      nullptr          ,
      &description_size,
      &order           ,
      &ticks_per_second,
      &maximum_ticks   ,
      &raw_information ))

    name       .resize(name_size);
    description.resize(description_size);

    MPI_CHECK_ERROR_CODE(MPI_T_source_get_info, (
      index             ,
      name       .data(), 
      &name_size        , 
      description.data(),
      &description_size ,
      &order            ,
      &ticks_per_second ,
      &maximum_ticks    ,
      &raw_information  ))

    ordered = order == MPI_T_SOURCE_ORDERED;
    if (raw_information != MPI_INFO_NULL)
      information.emplace(raw_information, true);
  }
  event_source           (const event_source&  that) = delete ;
  event_source           (      event_source&& temp) = default;
  virtual ~event_source  ()                          = default;
  event_source& operator=(const event_source&  that) = delete ;
  event_source& operator=(      event_source&& temp) = default;

  [[nodiscard]]
  count timestamp() const
  {
    count result;
    MPI_CHECK_ERROR_CODE(MPI_T_source_get_timestamp, (index, &result))
    return result;
  }

  std::int32_t               index           ;
                             
  std::string                name            ;
  std::string                description     ;
                             
  bool                       ordered         ;
  count                      ticks_per_second;
  count                      maximum_ticks   ;

  std::optional<information> information     ;
};

inline std::int32_t              event_source_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_source_get_num, (&result))
  return result;
}
inline std::vector<event_source> event_sources     ()
{
  const auto count = event_source_count();

  std::vector<event_source> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(i);

  return result;
}
inline std::vector<event_source> event_sources     (const std::vector<std::int32_t>& indices)
{
  const auto count = static_cast<std::int32_t>(indices.size());

  std::vector<event_source> result;
  result.reserve(count);

  for (auto i = 0; i < count; ++i)
    result.emplace_back(indices[i]);

  return result;
}
#endif
}
