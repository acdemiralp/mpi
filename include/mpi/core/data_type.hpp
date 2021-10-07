#pragma once

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
  data_type           (const data_type&  that) = delete;
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
  data_type& operator=(const data_type&  that) = delete;
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

protected:
  bool         managed_ = false;
  MPI_Datatype native_  = MPI_DATATYPE_NULL;
};



inline const data_type int_data_type                = data_type(MPI_INTEGER           );
inline const data_type unsigned_int_data_type       = data_type(MPI_UNSIGNED          );
inline const data_type unsigned_long_data_type      = data_type(MPI_UNSIGNED_LONG     );
inline const data_type unsigned_long_long_data_type = data_type(MPI_UNSIGNED_LONG_LONG);
inline const data_type char_data_type               = data_type(MPI_CHAR              );
inline const data_type float_data_type              = data_type(MPI_FLOAT             );
inline const data_type double_data_type             = data_type(MPI_DOUBLE            );
// TODO: Other common data types.



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

}