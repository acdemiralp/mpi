#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <mpi/core/enums/combiner.hpp>
#include <mpi/core/type/data_type_information.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
struct key_value
{
  key_value           () : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_keyval, (
      [ ] (MPI_Datatype type, std::int32_t index, void* state, void* in, void* out, std::int32_t* flag)
      {
        out   = in;
        *flag = 1 ;
        return MPI_SUCCESS;
      }, 
      [ ] (MPI_Datatype type, std::int32_t index, void* value, void* state)
      {
        return MPI_SUCCESS;
      }, 
      &index_, 
      nullptr)) // Extra state is unused.
  }
  key_value           (const key_value&  that) = delete;
  key_value           (      key_value&& temp) noexcept
  : managed_(temp.managed_), index_(temp.index_)
  {
    temp.managed_ = false;
    temp.index_   = MPI_KEYVAL_INVALID;
  }
  virtual ~key_value  ()
  {
    if (managed_ && index_ != MPI_KEYVAL_INVALID)
      MPI_CHECK_ERROR_CODE(MPI_Type_free_keyval, (&index_))
  }
  key_value& operator=(const key_value&  that) = delete;
  key_value& operator=(      key_value&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && index_ != MPI_KEYVAL_INVALID)
        MPI_CHECK_ERROR_CODE(MPI_Type_free_keyval, (&index_))

      managed_      = temp.managed_;
      index_        = temp.index_;

      temp.managed_ = false;
      temp.index_   = MPI_KEYVAL_INVALID;
    }
    return *this;
  }

protected:
  bool         managed_ = false;
  std::int32_t index_   = MPI_KEYVAL_INVALID;
};

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
  std::optional<type>      attribute       (const std::int32_t key) const
  {
    type         result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Type_get_attr, (native_, key, static_cast<void*>(&result), &exists))
    return exists ? result : std::optional<type>(std::nullopt);
  }
  template <typename type>
  void                     set_attribute   (const std::int32_t key, const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_set_attr, (native_, key, static_cast<void*>(&value)))
  }
  template <typename type>
  void                     remove_attribute(const std::int32_t key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_delete_attr, (native_, key))
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