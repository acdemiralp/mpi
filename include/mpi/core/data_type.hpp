#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <mpi/core/enums/combiner.hpp>
#include <mpi/core/exception.hpp>
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
    std::array<MPI_Aint, 2> result;
    MPI_CHECK_ERROR_CODE(MPI_Type_get_extent  , (native_, &result[0], &result[1]))
    return result;
  }
  [[nodiscard]]
  std::array<MPI_Count, 2> extent_x        () const
  {
    std::array<MPI_Count, 2> result;
    MPI_CHECK_ERROR_CODE(MPI_Type_get_extent_x, (native_, &result[0], &result[1]))
    return result;
  }
  [[nodiscard]]
  std::array<MPI_Aint , 2> true_extent     () const
  {
    std::array<MPI_Aint, 2> result;
    MPI_CHECK_ERROR_CODE(MPI_Type_get_true_extent  , (native_, &result[0], &result[1]))
    return result;
  }
  [[nodiscard]]
  std::array<MPI_Count, 2> true_extent_x   () const
  {
    std::array<MPI_Count, 2> result;
    MPI_CHECK_ERROR_CODE(MPI_Type_get_true_extent_x, (native_, &result[0], &result[1]))
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

template<typename type>
struct missing_get_data_type_implementation : std::false_type {};
template <typename type>
data_type get_data_type()
{
  static_assert(missing_get_data_type_implementation<type>::value, "Missing get_data_type implementation for type.");
  return data_type(MPI_DATATYPE_NULL);
}
#define MPI_SPECIALIZE_GET_DATA_TYPE(TYPE, DATA_TYPE) \
template <>                                           \
data_type get_data_type<TYPE>()                       \
{                                                     \
  return data_type(DATA_TYPE);                        \
}
MPI_SPECIALIZE_GET_DATA_TYPE(std::int16_t , MPI_SHORT             )
MPI_SPECIALIZE_GET_DATA_TYPE(std::uint16_t, MPI_UNSIGNED_SHORT    )
MPI_SPECIALIZE_GET_DATA_TYPE(std::int32_t , MPI_INTEGER           )
MPI_SPECIALIZE_GET_DATA_TYPE(std::uint32_t, MPI_UNSIGNED          )
MPI_SPECIALIZE_GET_DATA_TYPE(unsigned long, MPI_UNSIGNED_LONG     )
MPI_SPECIALIZE_GET_DATA_TYPE(std::uint64_t, MPI_UNSIGNED_LONG_LONG)
MPI_SPECIALIZE_GET_DATA_TYPE(char         , MPI_CHAR              )
MPI_SPECIALIZE_GET_DATA_TYPE(float        , MPI_FLOAT             )
MPI_SPECIALIZE_GET_DATA_TYPE(double       , MPI_DOUBLE            )
#undef MPI_SPECIALIZE_GET_DATA_TYPE

// Given MPI data type, cast to type??
template <MPI_Datatype data_type>
struct data_type_traits
{
  
};
  
#define MPI_SPECIALIZE_GET_DATA_TYPE(TYPE, DATA_TYPE) \
template <>                                           \
data_type get_data_type<TYPE>()                       \
{                                                     \
  return data_type(DATA_TYPE);                        \
}

template <>
struct data_type_traits<MPI_SHORT>
{
  using type = std::int16_t;
};
template <>
struct data_type_traits<MPI_UNSIGNED_SHORT>
{
  using type = std::uint16_t;
};

}