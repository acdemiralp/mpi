#pragma once

#include <complex>
#include <cstddef>
#include <cstdint>

#include <mpi/core/type/data_type.hpp>
#include <mpi/core/utility/missing_implementation.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
// Given a typename, retrieves the associated MPI data type at compile or runtime.
template <typename type>
data_type get_data_type()
{
  static_assert(missing_implementation<type>::value, "Missing get_data_type implementation for type.");
  return data_type(MPI_DATATYPE_NULL);
}

template<> inline data_type get_data_type<char                     >() { return data_type(MPI_CHAR                   ); }
template<> inline data_type get_data_type<short                    >() { return data_type(MPI_SHORT                  ); }
template<> inline data_type get_data_type<int                      >() { return data_type(MPI_INT                    ); }
template<> inline data_type get_data_type<long                     >() { return data_type(MPI_LONG                   ); }
template<> inline data_type get_data_type<long long                >() { return data_type(MPI_LONG_LONG              ); }
template<> inline data_type get_data_type<signed char              >() { return data_type(MPI_SIGNED_CHAR            ); }
template<> inline data_type get_data_type<unsigned char            >() { return data_type(MPI_UNSIGNED_CHAR          ); }
template<> inline data_type get_data_type<unsigned short           >() { return data_type(MPI_UNSIGNED_SHORT         ); }
template<> inline data_type get_data_type<unsigned int             >() { return data_type(MPI_UNSIGNED               ); }
template<> inline data_type get_data_type<unsigned long            >() { return data_type(MPI_UNSIGNED_LONG          ); }
template<> inline data_type get_data_type<unsigned long long       >() { return data_type(MPI_UNSIGNED_LONG_LONG     ); }
template<> inline data_type get_data_type<float                    >() { return data_type(MPI_FLOAT                  ); }
template<> inline data_type get_data_type<double                   >() { return data_type(MPI_DOUBLE                 ); }
template<> inline data_type get_data_type<long double              >() { return data_type(MPI_LONG_DOUBLE            ); }
template<> inline data_type get_data_type<wchar_t                  >() { return data_type(MPI_WCHAR                  ); }
//template<> inline data_type get_data_type<_Bool                  >() { return data_type(MPI_C_BOOL                 ); }
//template<> inline data_type get_data_type<std::int8_t            >() { return data_type(MPI_INT8_T                 ); }
//template<> inline data_type get_data_type<std::int16_t           >() { return data_type(MPI_INT16_T                ); }
//template<> inline data_type get_data_type<std::int32_t           >() { return data_type(MPI_INT32_T                ); }
//template<> inline data_type get_data_type<std::int64_t           >() { return data_type(MPI_INT64_T                ); }
//template<> inline data_type get_data_type<std::uint8_t           >() { return data_type(MPI_UINT8_T                ); }
//template<> inline data_type get_data_type<std::uint16_t          >() { return data_type(MPI_UINT16_T               ); }
//template<> inline data_type get_data_type<std::uint32_t          >() { return data_type(MPI_UINT32_T               ); }
//template<> inline data_type get_data_type<std::uint64_t          >() { return data_type(MPI_UINT64_T               ); }
//template<> inline data_type get_data_type<MPI_Aint               >() { return data_type(MPI_AINT                   ); }
//template<> inline data_type get_data_type<MPI_Count              >() { return data_type(MPI_COUNT                  ); }
//template<> inline data_type get_data_type<MPI_Offset             >() { return data_type(MPI_OFFSET                 ); }
//template<> inline data_type get_data_type<float       _Complex   >() { return data_type(MPI_C_FLOAT_COMPLEX        ); }
//template<> inline data_type get_data_type<double      _Complex   >() { return data_type(MPI_C_DOUBLE_COMPLEX       ); }
//template<> inline data_type get_data_type<long double _Complex   >() { return data_type(MPI_C_LONG_DOUBLE_COMPLEX  ); }
template<> inline data_type get_data_type<std::byte                >() { return data_type(MPI_BYTE                   ); }
//template<> inline data_type get_data_type<_Packed                >() { return data_type(MPI_PACKED                 ); }
template<> inline data_type get_data_type<bool                     >() { return data_type(MPI_CXX_BOOL               ); }
template<> inline data_type get_data_type<std::complex<float      >>() { return data_type(MPI_CXX_FLOAT_COMPLEX      ); }
template<> inline data_type get_data_type<std::complex<double     >>() { return data_type(MPI_CXX_DOUBLE_COMPLEX     ); }
template<> inline data_type get_data_type<std::complex<long double>>() { return data_type(MPI_CXX_LONG_DOUBLE_COMPLEX); }

// Given a MPI data type, retrieves the associated typename at compile time.
template <MPI_Datatype data_type>
struct data_type_traits
{

};

template <> struct data_type_traits<MPI_CHAR                   > { using type = char                     ; };
template <> struct data_type_traits<MPI_SHORT                  > { using type = std::int16_t             ; };
template <> struct data_type_traits<MPI_INT                    > { using type = std::int32_t             ; };
template <> struct data_type_traits<MPI_LONG                   > { using type = long                     ; };
template <> struct data_type_traits<MPI_LONG_LONG              > { using type = std::int64_t             ; };
template <> struct data_type_traits<MPI_SIGNED_CHAR            > { using type = std::int8_t              ; };
template <> struct data_type_traits<MPI_UNSIGNED_CHAR          > { using type = std::uint8_t             ; };
template <> struct data_type_traits<MPI_UNSIGNED_SHORT         > { using type = std::uint16_t            ; };
template <> struct data_type_traits<MPI_UNSIGNED               > { using type = std::uint32_t            ; };
template <> struct data_type_traits<MPI_UNSIGNED_LONG          > { using type = unsigned long            ; };
template <> struct data_type_traits<MPI_UNSIGNED_LONG_LONG     > { using type = std::uint64_t            ; };
template <> struct data_type_traits<MPI_FLOAT                  > { using type = float                    ; };
template <> struct data_type_traits<MPI_DOUBLE                 > { using type = double                   ; };
template <> struct data_type_traits<MPI_LONG_DOUBLE            > { using type = long double              ; };
template <> struct data_type_traits<MPI_WCHAR                  > { using type = wchar_t                  ; };
//template <> struct data_type_traits<MPI_C_BOOL               > { using type = _Bool                    ; };
template <> struct data_type_traits<MPI_INT8_T                 > { using type = std::int8_t              ; };
template <> struct data_type_traits<MPI_INT16_T                > { using type = std::int16_t             ; };
template <> struct data_type_traits<MPI_INT32_T                > { using type = std::int32_t             ; };
template <> struct data_type_traits<MPI_INT64_T                > { using type = std::int64_t             ; };
template <> struct data_type_traits<MPI_UINT8_T                > { using type = std::uint8_t             ; };
template <> struct data_type_traits<MPI_UINT16_T               > { using type = std::uint16_t            ; };
template <> struct data_type_traits<MPI_UINT32_T               > { using type = std::uint32_t            ; };
template <> struct data_type_traits<MPI_UINT64_T               > { using type = std::uint64_t            ; };
template <> struct data_type_traits<MPI_AINT                   > { using type = std::int64_t             ; };
template <> struct data_type_traits<MPI_COUNT                  > { using type = std::int64_t             ; };
template <> struct data_type_traits<MPI_OFFSET                 > { using type = std::int64_t             ; };
//template <> struct data_type_traits<MPI_C_FLOAT_COMPLEX      > { using type = float       _Complex     ; };
//template <> struct data_type_traits<MPI_C_DOUBLE_COMPLEX     > { using type = double      _Complex     ; };
//template <> struct data_type_traits<MPI_C_LONG_DOUBLE_COMPLEX> { using type = long double _Complex     ; };
template <> struct data_type_traits<MPI_BYTE                   > { using type = std::byte                ; };
//template <> struct data_type_traits<MPI_PACKED               > { using type = _Packed                  ; };
template <> struct data_type_traits<MPI_CXX_BOOL               > { using type = bool                     ; };
template <> struct data_type_traits<MPI_CXX_FLOAT_COMPLEX      > { using type = std::complex<float      >; };
template <> struct data_type_traits<MPI_CXX_DOUBLE_COMPLEX     > { using type = std::complex<double     >; };
template <> struct data_type_traits<MPI_CXX_LONG_DOUBLE_COMPLEX> { using type = std::complex<long double>; };
}