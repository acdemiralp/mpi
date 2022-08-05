#pragma once

#include <complex>
#include <cstddef>
#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
template <MPI_Datatype data_type>
struct data_type_traits {};

template <> struct data_type_traits<MPI_CHAR                   > { using type = char                     ; };
template <> struct data_type_traits<MPI_SHORT                  > { using type = short                    ; };
template <> struct data_type_traits<MPI_INT                    > { using type = int                      ; };
template <> struct data_type_traits<MPI_LONG                   > { using type = long                     ; };
template <> struct data_type_traits<MPI_LONG_LONG              > { using type = long long                ; };
template <> struct data_type_traits<MPI_SIGNED_CHAR            > { using type = signed char              ; };
template <> struct data_type_traits<MPI_UNSIGNED_CHAR          > { using type = unsigned char            ; };
template <> struct data_type_traits<MPI_UNSIGNED_SHORT         > { using type = unsigned short           ; };
template <> struct data_type_traits<MPI_UNSIGNED               > { using type = unsigned                 ; };
template <> struct data_type_traits<MPI_UNSIGNED_LONG          > { using type = unsigned long            ; };
template <> struct data_type_traits<MPI_UNSIGNED_LONG_LONG     > { using type = unsigned long long       ; };
template <> struct data_type_traits<MPI_FLOAT                  > { using type = float                    ; };
template <> struct data_type_traits<MPI_DOUBLE                 > { using type = double                   ; };
template <> struct data_type_traits<MPI_LONG_DOUBLE            > { using type = long double              ; };
template <> struct data_type_traits<MPI_WCHAR                  > { using type = wchar_t                  ; };
template <> struct data_type_traits<MPI_INT8_T                 > { using type = std::int8_t              ; };
template <> struct data_type_traits<MPI_INT16_T                > { using type = std::int16_t             ; };
template <> struct data_type_traits<MPI_INT32_T                > { using type = std::int32_t             ; };
template <> struct data_type_traits<MPI_INT64_T                > { using type = std::int64_t             ; };
template <> struct data_type_traits<MPI_UINT8_T                > { using type = std::uint8_t             ; };
template <> struct data_type_traits<MPI_UINT16_T               > { using type = std::uint16_t            ; };
template <> struct data_type_traits<MPI_UINT32_T               > { using type = std::uint32_t            ; };
template <> struct data_type_traits<MPI_UINT64_T               > { using type = std::uint64_t            ; };
template <> struct data_type_traits<MPI_AINT                   > { using type = aint                     ; };
template <> struct data_type_traits<MPI_COUNT                  > { using type = count                    ; };
template <> struct data_type_traits<MPI_OFFSET                 > { using type = offset                   ; };
template <> struct data_type_traits<MPI_BYTE                   > { using type = std::byte                ; };
template <> struct data_type_traits<MPI_CXX_BOOL               > { using type = bool                     ; };
template <> struct data_type_traits<MPI_CXX_FLOAT_COMPLEX      > { using type = std::complex<float      >; };
template <> struct data_type_traits<MPI_CXX_DOUBLE_COMPLEX     > { using type = std::complex<double     >; };
template <> struct data_type_traits<MPI_CXX_LONG_DOUBLE_COMPLEX> { using type = std::complex<long double>; };
//template <> struct data_type_traits<MPI_C_BOOL               > { using type = _Bool                    ; };
//template <> struct data_type_traits<MPI_C_FLOAT_COMPLEX      > { using type = float       _Complex     ; };
//template <> struct data_type_traits<MPI_C_DOUBLE_COMPLEX     > { using type = double      _Complex     ; };
//template <> struct data_type_traits<MPI_C_LONG_DOUBLE_COMPLEX> { using type = long double _Complex     ; };
//template <> struct data_type_traits<MPI_PACKED               > { using type = _Packed                  ; };
}