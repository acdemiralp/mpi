#pragma once

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/tool/structs/performance_variable_information.hpp>
#include <mpi/tool/session.hpp>

namespace mpi::tool
{
class performance_variable
{
public:
  template <typename type>
  explicit performance_variable  (const session& session, const std::int32_t index, type& data)
  : session_(session)
  {
    std::int32_t count;
    MPI_T_pvar_handle_alloc(session_.native(), index, static_cast<void*>(&data), &native_, &count);
  }
  performance_variable           (const performance_variable&  that) = delete;
  performance_variable           (      performance_variable&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_), session_(temp.session_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_T_PVAR_HANDLE_NULL;
  }
  virtual ~performance_variable  ()
  {
    if (managed_ && native_ != MPI_T_PVAR_HANDLE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_free, (session_.native(), &native_))
  }
  performance_variable& operator=(const performance_variable&  that) = delete;
  performance_variable& operator=(      performance_variable&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_T_PVAR_HANDLE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_T_pvar_handle_free, (session_.native(), &native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_T_PVAR_HANDLE_NULL;
    }
    return *this;
  }

  void              start     () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_start, (session_.native(), native_))
  }
  void              stop      () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_stop , (session_.native(), native_))
  }
  void              reset     () const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_reset, (session_.native(), native_))
  }

  template <typename type>
  type              read      () const
  {
    type result;
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_read     , (session_.native(), native_, static_cast<      void*>(&result)))
    return result;
  }
  template <typename type>
  type              read_reset() const
  {
    type result;
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_readreset, (session_.native(), native_, static_cast<      void*>(&result)))
    return result;
  }
  template <typename type>
  void              write     (const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_T_pvar_write    , (session_.native(), native_, static_cast<const void*>(&value)))
  }

  [[nodiscard]]
  bool              managed   () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_T_pvar_handle native    () const
  {
    return native_;
  }

protected:
  bool              managed_ = false;
  MPI_T_pvar_handle native_  = MPI_T_PVAR_HANDLE_NULL;
  const session&    session_ ;
};

inline std::int32_t                                  performance_variable_count()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_num, (&result))
  return result;
}
inline std::vector<performance_variable_information> performance_variable_info ()
{
  std::vector<performance_variable_information> result(performance_variable_count());

  for (std::size_t i = 0; i < result.size(); ++i)
  {
    auto& info = result[i];

    auto         name_size(0), description_size(0);
    MPI_Datatype data_type;
    MPI_T_enum   enum_type;

    MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_info, (
      static_cast<std::int32_t>(i)                         , 
      nullptr                                              , 
      &name_size                                           , 
      reinterpret_cast<std::int32_t*>(&info.verbosity)     ,
      reinterpret_cast<std::int32_t*>(&info.variable_class),
      &data_type                                           ,
      &enum_type                                           ,
      nullptr                                              ,
      &description_size                                    ,
      reinterpret_cast<std::int32_t*>(&info.bind_type)     ,
      reinterpret_cast<std::int32_t*>(&info.read_only)     ,
      reinterpret_cast<std::int32_t*>(&info.continuous)    ,
      reinterpret_cast<std::int32_t*>(&info.atomic)        ))

    info.name       .resize(name_size);
    info.description.resize(description_size);
    info.data_type = mpi::data_type(data_type);

    MPI_CHECK_ERROR_CODE(MPI_T_pvar_get_info, (
      static_cast<std::int32_t>(i)                         , 
      info.name       .data()                              , 
      &name_size                                           , 
      reinterpret_cast<std::int32_t*>(&info.verbosity)     ,
      reinterpret_cast<std::int32_t*>(&info.variable_class),
      &data_type                                           ,
      &enum_type                                           ,
      info.description.data()                              ,
      &description_size                                    ,
      reinterpret_cast<std::int32_t*>(&info.bind_type)     ,
      reinterpret_cast<std::int32_t*>(&info.read_only)     ,
      reinterpret_cast<std::int32_t*>(&info.continuous)    ,
      reinterpret_cast<std::int32_t*>(&info.atomic)        ))

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