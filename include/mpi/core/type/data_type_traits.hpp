#pragma once

#include <complex>
#include <cstddef>
#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi
{
// Maps C++ types to their corresponding MPI_Datatype values at runtime.
// Similar to type_traits but returns raw MPI_Datatype instead of data_type wrapper.
// Note: Since MPI constants are not compile-time constants in all MPI implementations,
// we cannot use them as non-type template parameters.
template <typename type>
struct data_type_traits;

template <> struct data_type_traits<char                     > { static MPI_Datatype mpi_type() { return MPI_CHAR                   ; } };
template <> struct data_type_traits<short                    > { static MPI_Datatype mpi_type() { return MPI_SHORT                  ; } };
template <> struct data_type_traits<int                      > { static MPI_Datatype mpi_type() { return MPI_INT                    ; } };
template <> struct data_type_traits<long                     > { static MPI_Datatype mpi_type() { return MPI_LONG                   ; } };
template <> struct data_type_traits<long long                > { static MPI_Datatype mpi_type() { return MPI_LONG_LONG              ; } };
template <> struct data_type_traits<signed char              > { static MPI_Datatype mpi_type() { return MPI_SIGNED_CHAR            ; } };
template <> struct data_type_traits<unsigned char            > { static MPI_Datatype mpi_type() { return MPI_UNSIGNED_CHAR          ; } };
template <> struct data_type_traits<unsigned short           > { static MPI_Datatype mpi_type() { return MPI_UNSIGNED_SHORT         ; } };
template <> struct data_type_traits<unsigned                 > { static MPI_Datatype mpi_type() { return MPI_UNSIGNED               ; } };
template <> struct data_type_traits<unsigned long            > { static MPI_Datatype mpi_type() { return MPI_UNSIGNED_LONG          ; } };
template <> struct data_type_traits<unsigned long long       > { static MPI_Datatype mpi_type() { return MPI_UNSIGNED_LONG_LONG     ; } };
template <> struct data_type_traits<float                    > { static MPI_Datatype mpi_type() { return MPI_FLOAT                  ; } };
template <> struct data_type_traits<double                   > { static MPI_Datatype mpi_type() { return MPI_DOUBLE                 ; } };
template <> struct data_type_traits<long double              > { static MPI_Datatype mpi_type() { return MPI_LONG_DOUBLE            ; } };
template <> struct data_type_traits<wchar_t                  > { static MPI_Datatype mpi_type() { return MPI_WCHAR                  ; } };
template <> struct data_type_traits<std::byte                > { static MPI_Datatype mpi_type() { return MPI_BYTE                   ; } };
template <> struct data_type_traits<bool                     > { static MPI_Datatype mpi_type() { return MPI_CXX_BOOL               ; } };
template <> struct data_type_traits<std::complex<float      >> { static MPI_Datatype mpi_type() { return MPI_CXX_FLOAT_COMPLEX      ; } };
template <> struct data_type_traits<std::complex<double     >> { static MPI_Datatype mpi_type() { return MPI_CXX_DOUBLE_COMPLEX     ; } };
template <> struct data_type_traits<std::complex<long double>> { static MPI_Datatype mpi_type() { return MPI_CXX_LONG_DOUBLE_COMPLEX; } };

// Note: We don't provide specializations for std::int8_t, std::int16_t, std::int32_t, std::int64_t, 
// std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, aint, count, or offset here because these
// types are typically aliases (typedefs) for the fundamental types above on most platforms.
// Users should use the fundamental types directly with this traits class, or use type_traits instead
// which handles these cases properly at runtime.

//template <> struct data_type_traits<_Bool                    > { static MPI_Datatype mpi_type() { return MPI_C_BOOL                 ; } };
//template <> struct data_type_traits<float       _Complex     > { static MPI_Datatype mpi_type() { return MPI_C_FLOAT_COMPLEX        ; } };
//template <> struct data_type_traits<double      _Complex     > { static MPI_Datatype mpi_type() { return MPI_C_DOUBLE_COMPLEX       ; } };
//template <> struct data_type_traits<long double _Complex     > { static MPI_Datatype mpi_type() { return MPI_C_LONG_DOUBLE_COMPLEX  ; } };
//template <> struct data_type_traits<_Packed                  > { static MPI_Datatype mpi_type() { return MPI_PACKED                 ; } };
}