#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/control_variable_information.hpp>

namespace mpi::tool
{
class control_variable
{
public:
  template <typename type>
  explicit control_variable  (const std::int32_t index, type& data)
  : managed_(true)
  {
    std::int32_t count;
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_alloc, (index, static_cast<void*>(&data), &native_, &count))
  }
  explicit control_variable  (const MPI_T_cvar_handle& native)
  : native_(native)
  {
    
  }
  control_variable           (const control_variable&  that) = delete;
  control_variable           (      control_variable&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_CVAR_HANDLE_NULL;
  }
  virtual ~control_variable  ()
  {
    if (managed_ && native_ != MPI_T_CVAR_HANDLE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_free, (&native_))
  }
  control_variable& operator=(const control_variable&  that) = delete;
  control_variable& operator=(      control_variable&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_T_CVAR_HANDLE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_T_cvar_handle_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_T_CVAR_HANDLE_NULL;
    }
    return *this;
  }

  template <typename type>
  type read () const
  {
    type result;
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_read, (native_, static_cast<void*>(&result)))
    return result;
  }
  template <typename type>
  void write(const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_cvar_write, (native_, static_cast<const void*>(&value)))
  }

protected:
  bool              managed_ = false;
  MPI_T_cvar_handle native_  = MPI_T_CVAR_HANDLE_NULL;
};

inline std::int32_t                              control_variable_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_num, (&result))
  return result;
}
inline std::vector<control_variable_information> control_variable_info ()
{
  std::vector<control_variable_information> result(control_variable_count());

  for (std::size_t i = 0; i < result.size(); ++i)
  {
    auto& info = result[i];

    auto         name_size(0), description_size(0);
    MPI_Datatype data_type;
    MPI_T_enum   enum_type;

    MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_info, (
      static_cast<std::int32_t>(i)                    , 
      nullptr                                         , 
      &name_size                                      , 
      reinterpret_cast<std::int32_t*>(&info.verbosity),
      &data_type                                      ,
      &enum_type                                      ,
      nullptr                                         ,
      &description_size                               ,
      reinterpret_cast<std::int32_t*>(&info.bind_type),
      reinterpret_cast<std::int32_t*>(&info.scope)))

    info.name       .resize(name_size);
    info.description.resize(description_size);
    info.data_type = mpi::data_type(data_type);

    MPI_CHECK_ERROR_CODE(MPI_T_cvar_get_info, (
      static_cast<std::int32_t>(i)                    , 
      info.name.data()                                ,
      &name_size                                      , 
      reinterpret_cast<std::int32_t*>(&info.verbosity),
      &data_type                                      ,
      &enum_type                                      ,
      info.description.data()                         ,
      &description_size                               ,
      reinterpret_cast<std::int32_t*>(&info.bind_type),
      reinterpret_cast<std::int32_t*>(&info.scope)))

    if (enum_type != MPI_T_ENUM_NULL)
    {
      auto& enum_info = result[i].enum_information.emplace();
      enum_info.type  = enum_type;

      auto enum_length(0), name_length(0);

      MPI_CHECK_ERROR_CODE(MPI_T_enum_get_info, (enum_type, &enum_length, nullptr, &name_length))

      enum_info.name .resize(name_length);
      enum_info.items.resize(enum_length);

      MPI_CHECK_ERROR_CODE(MPI_T_enum_get_info, (enum_type, &enum_length, result[i].enum_information->name.data(), &name_length))

      for (std::size_t j = 0; j < enum_info.items.size(); ++j)
      {
        auto& enum_item = enum_info.items[j];
        enum_item.index = static_cast<std::int32_t>(j);

        MPI_CHECK_ERROR_CODE(MPI_T_enum_get_item, (enum_type, enum_item.index, &enum_item.value, nullptr, &name_length))

        enum_item.name.resize(name_length);

        MPI_CHECK_ERROR_CODE(MPI_T_enum_get_item, (enum_type, enum_item.index, &enum_item.value, enum_item.name.data(), &name_length))
      }
    }
  }

  return result;
}
}