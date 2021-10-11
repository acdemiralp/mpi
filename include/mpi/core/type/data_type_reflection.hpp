#pragma once

#include <array>
#include <complex>
#include <cstddef>
#include <cstdint>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/utility/missing_implementation.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
// Given a typename, retrieves the associated MPI data type at runtime.
template <typename type>
struct type_traits
{
static data_type get_data_type()
{
  static_assert(missing_implementation<type>::value, "Missing get_data_type implementation for type.");
  return data_type(MPI_DATATYPE_NULL);
}
};

#define MPI_SPECIALIZE_TYPE_TRAITS(TYPE, DATA_TYPE) \
template <>                                         \
struct type_traits<TYPE>                            \
{                                                   \
static data_type get_data_type()                    \
{                                                   \
  return data_type(DATA_TYPE);                      \
}                                                   \
};
MPI_SPECIALIZE_TYPE_TRAITS(char                     , MPI_CHAR                   )
MPI_SPECIALIZE_TYPE_TRAITS(short                    , MPI_SHORT                  )
MPI_SPECIALIZE_TYPE_TRAITS(int                      , MPI_INT                    )
MPI_SPECIALIZE_TYPE_TRAITS(long                     , MPI_LONG                   )
MPI_SPECIALIZE_TYPE_TRAITS(long long                , MPI_LONG_LONG              )
MPI_SPECIALIZE_TYPE_TRAITS(signed char              , MPI_SIGNED_CHAR            )
MPI_SPECIALIZE_TYPE_TRAITS(unsigned char            , MPI_UNSIGNED_CHAR          )
MPI_SPECIALIZE_TYPE_TRAITS(unsigned short           , MPI_UNSIGNED_SHORT         )
MPI_SPECIALIZE_TYPE_TRAITS(unsigned int             , MPI_UNSIGNED               )
MPI_SPECIALIZE_TYPE_TRAITS(unsigned long            , MPI_UNSIGNED_LONG          )
MPI_SPECIALIZE_TYPE_TRAITS(unsigned long long       , MPI_UNSIGNED_LONG_LONG     )
MPI_SPECIALIZE_TYPE_TRAITS(float                    , MPI_FLOAT                  )
MPI_SPECIALIZE_TYPE_TRAITS(double                   , MPI_DOUBLE                 )
MPI_SPECIALIZE_TYPE_TRAITS(long double              , MPI_LONG_DOUBLE            )
MPI_SPECIALIZE_TYPE_TRAITS(wchar_t                  , MPI_WCHAR                  )
//MPI_SPECIALIZE_TYPE_TRAITS(_Bool                  , MPI_C_BOOL                 )
//MPI_SPECIALIZE_TYPE_TRAITS(std::int8_t            , MPI_INT8_T                 )
//MPI_SPECIALIZE_TYPE_TRAITS(std::int16_t           , MPI_INT16_T                )
//MPI_SPECIALIZE_TYPE_TRAITS(std::int32_t           , MPI_INT32_T                )
//MPI_SPECIALIZE_TYPE_TRAITS(std::int64_t           , MPI_INT64_T                )
//MPI_SPECIALIZE_TYPE_TRAITS(std::uint8_t           , MPI_UINT8_T                )
//MPI_SPECIALIZE_TYPE_TRAITS(std::uint16_t          , MPI_UINT16_T               )
//MPI_SPECIALIZE_TYPE_TRAITS(std::uint32_t          , MPI_UINT32_T               )
//MPI_SPECIALIZE_TYPE_TRAITS(std::uint64_t          , MPI_UINT64_T               )
//MPI_SPECIALIZE_TYPE_TRAITS(MPI_Aint               , MPI_AINT                   )
//MPI_SPECIALIZE_TYPE_TRAITS(MPI_Count              , MPI_COUNT                  )
//MPI_SPECIALIZE_TYPE_TRAITS(MPI_Offset             , MPI_OFFSET                 )
//MPI_SPECIALIZE_TYPE_TRAITS(float       _Complex   , MPI_C_FLOAT_COMPLEX        )
//MPI_SPECIALIZE_TYPE_TRAITS(double      _Complex   , MPI_C_DOUBLE_COMPLEX       )
//MPI_SPECIALIZE_TYPE_TRAITS(long double _Complex   , MPI_C_LONG_DOUBLE_COMPLEX  )
MPI_SPECIALIZE_TYPE_TRAITS(std::byte                , MPI_BYTE                   )
//MPI_SPECIALIZE_TYPE_TRAITS(_Packed                , MPI_PACKED                 )
MPI_SPECIALIZE_TYPE_TRAITS(bool                     , MPI_CXX_BOOL               )
MPI_SPECIALIZE_TYPE_TRAITS(std::complex<float      >, MPI_CXX_FLOAT_COMPLEX      )
MPI_SPECIALIZE_TYPE_TRAITS(std::complex<double     >, MPI_CXX_DOUBLE_COMPLEX     )
MPI_SPECIALIZE_TYPE_TRAITS(std::complex<long double>, MPI_CXX_LONG_DOUBLE_COMPLEX)
#undef MPI_SPECIALIZE_TYPE_TRAITS

// Contiguous basic type traits.
template <typename type, std::size_t size>
struct type_traits<type[size]>
{
  static data_type get_data_type()
  {
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};
template <typename type, std::size_t size>
struct type_traits<std::array<type, size>>
{
  static data_type get_data_type()
  {
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};

// Given a MPI data type, retrieves the associated typename at compile time.
template <MPI_Datatype data_type>
struct data_type_traits
{

};

#define MPI_SPECIALIZE_DATA_TYPE_TRAITS(TYPE, DATA_TYPE) \
template <>                                              \
struct data_type_traits<DATA_TYPE>                       \
{                                                        \
using type = TYPE;                                       \
};
MPI_SPECIALIZE_DATA_TYPE_TRAITS(char                     , MPI_CHAR                   )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int16_t             , MPI_SHORT                  )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int32_t             , MPI_INT                    )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(long                     , MPI_LONG                   )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int64_t             , MPI_LONG_LONG              )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int8_t              , MPI_SIGNED_CHAR            )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint8_t             , MPI_UNSIGNED_CHAR          )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint16_t            , MPI_UNSIGNED_SHORT         )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint32_t            , MPI_UNSIGNED               )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(unsigned long            , MPI_UNSIGNED_LONG          )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint64_t            , MPI_UNSIGNED_LONG_LONG     )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(float                    , MPI_FLOAT                  )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(double                   , MPI_DOUBLE                 )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(long double              , MPI_LONG_DOUBLE            )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(wchar_t                  , MPI_WCHAR                  )
//MPI_SPECIALIZE_DATA_TYPE_TRAITS(_Bool                  , MPI_C_BOOL                 )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int8_t              , MPI_INT8_T                 )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int16_t             , MPI_INT16_T                )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int32_t             , MPI_INT32_T                )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int64_t             , MPI_INT64_T                )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint8_t             , MPI_UINT8_T                )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint16_t            , MPI_UINT16_T               )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint32_t            , MPI_UINT32_T               )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::uint64_t            , MPI_UINT64_T               )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int64_t             , MPI_AINT                   )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int64_t             , MPI_COUNT                  )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::int64_t             , MPI_OFFSET                 )
//MPI_SPECIALIZE_DATA_TYPE_TRAITS(float       _Complex   , MPI_C_FLOAT_COMPLEX        )
//MPI_SPECIALIZE_DATA_TYPE_TRAITS(double      _Complex   , MPI_C_DOUBLE_COMPLEX       )
//MPI_SPECIALIZE_DATA_TYPE_TRAITS(long double _Complex   , MPI_C_LONG_DOUBLE_COMPLEX  )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::byte                , MPI_BYTE                   )
//MPI_SPECIALIZE_DATA_TYPE_TRAITS(_Packed                , MPI_PACKED                 )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(bool                     , MPI_CXX_BOOL               )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::complex<float      >, MPI_CXX_FLOAT_COMPLEX      )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::complex<double     >, MPI_CXX_DOUBLE_COMPLEX     )
MPI_SPECIALIZE_DATA_TYPE_TRAITS(std::complex<long double>, MPI_CXX_LONG_DOUBLE_COMPLEX)
#undef MPI_SPECIALIZE_DATA_TYPE_TRAITS
}