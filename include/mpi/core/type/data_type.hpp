#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>

#include <mpi/core/enums/combiner.hpp>
#include <mpi/core/type/data_type_information.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/key_value.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class data_type
{
public:
  explicit data_type  (const MPI_Datatype native)
  : native_(native)
  {
    
  }
  data_type           (const data_type&  that)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_dup, (that.native_, &native_))
  }
  data_type           (      data_type&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_DATATYPE_NULL;
  }
  virtual ~data_type  ()
  {
    if (managed_ && native_ != MPI_DATATYPE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Type_free, (&native_))
  }
  data_type& operator=(const data_type&  that)
  {
    if (this != &that)
    {
      if (managed_ && native_ != MPI_DATATYPE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Type_free, (&native_))
      
      managed_ = true;
      MPI_CHECK_ERROR_CODE(MPI_Type_dup, (that.native_, &native_))
    }
    return *this;
  }
  data_type& operator=(      data_type&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_DATATYPE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Type_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_DATATYPE_NULL;
    }
    return *this;
  }

  [[nodiscard]]
  std::string              name            () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, ' ');
    std::int32_t count (0);
    MPI_CHECK_ERROR_CODE(MPI_Type_get_name, (native_, result.data(), &count))
    result.resize(count);
    return result;
  }
  void                     set_name        (const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_set_name, (native_, value.data()))
  }

  template <typename type>
  std::optional<type>      attribute       (const communicator_key_value& key) const
  {
    type         result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Type_get_attr   , (native_, key.native(), static_cast<void*>(&result), &exists))
    return static_cast<bool>(exists) ? result : std::optional<type>(std::nullopt);
  }
  template <typename type>
  void                     set_attribute   (const communicator_key_value& key, const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_set_attr   , (native_, key.native(), static_cast<void*>(&value)))
  }
  void                     remove_attribute(const communicator_key_value& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_delete_attr, (native_, key.native()))
  }

  [[nodiscard]]
  std::int32_t             size            () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Type_size, (native_, &result))
    return result;
  }
  [[nodiscard]]
  MPI_Count                size_x          () const
  {
    MPI_Count result;
    MPI_CHECK_ERROR_CODE(MPI_Type_size_x, (native_, &result))
    return result;
  }

  [[nodiscard]]
  std::array<MPI_Aint , 2> extent          () const
  {
    std::array<MPI_Aint, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_extent  , (native_, &result[0], &result[1]))
    return result;
  }
  [[nodiscard]]
  std::array<MPI_Count, 2> extent_x        () const
  {
    std::array<MPI_Count, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_extent_x, (native_, &result[0], &result[1]))
    return result;
  }

  [[nodiscard]]
  std::array<MPI_Aint , 2> true_extent     () const
  {
    std::array<MPI_Aint, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_true_extent  , (native_, &result[0], &result[1]))
    return result;
  }
  [[nodiscard]]
  std::array<MPI_Count, 2> true_extent_x   () const
  {
    std::array<MPI_Count, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_true_extent_x, (native_, &result[0], &result[1]))
    return result;
  }

  [[nodiscard]]
  data_type_information    information     () const
  {
    data_type_information result;

    std::int32_t integers_size, addresses_size, data_types_size;

    MPI_CHECK_ERROR_CODE(MPI_Type_get_envelope, (
      native_         , 
      &integers_size  , 
      &addresses_size , 
      &data_types_size, 
      reinterpret_cast<std::int32_t*>(&result.combiner)))

    result.integers  .resize(integers_size  );
    result.addresses .resize(addresses_size );
    result.data_types.resize(data_types_size);

    MPI_CHECK_ERROR_CODE(MPI_Type_get_contents, (
      native_                 ,
      integers_size           ,
      addresses_size          ,
      data_types_size         ,
      result.integers  .data(),
      result.addresses .data(),
      result.data_types.data()))

    return result;
  }

  void                     commit          ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_commit, (&native_))
  }

  [[nodiscard]]
  bool                     managed         () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Datatype             native          () const
  {
    return native_;
  }

protected:
  bool         managed_ = false;
  MPI_Datatype native_  = MPI_DATATYPE_NULL;
};
}