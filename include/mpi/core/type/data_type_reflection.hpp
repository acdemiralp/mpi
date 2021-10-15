#pragma once

#include <array>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <mpi/core/type/compliant_traits.hpp>
#include <mpi/core/type/data_type.hpp>
#include <mpi/core/utility/array_traits.hpp>
#include <mpi/core/utility/missing_implementation.hpp>
#include <mpi/core/utility/span_traits.hpp>
#include <mpi/core/utility/tuple_traits.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/third_party/pfr.hpp>

// Limitations of automatic data type inference:
//
// The class/struct must be an aggregate:
// - No user-declared or inherited constructors.
// - No private or protected direct non-static data members.
// - No virtual functions.
// - No virtual, private or protected base classes.
// Additionally:
// - No const fields.
// - No references.
// - Static data members are ignored.
//
// See https://www.boost.org/doc/libs/1_76_0/doc/html/boost_pfr/limitations_and_configuration.html for further detail.
namespace mpi
{
// Given a typename, retrieves the associated MPI data type.
template <typename type, typename = void>
struct type_traits;

// Specialization for arithmetic types (using standard MPI types).
template <typename type>
struct type_traits<type, std::enable_if_t<std::is_arithmetic_v<type>>>
{
  static data_type get_data_type()
  {
    if      constexpr (std::is_same_v<type, char                     >) return data_type(MPI_CHAR                   );
    else if constexpr (std::is_same_v<type, short                    >) return data_type(MPI_SHORT                  );
    else if constexpr (std::is_same_v<type, int                      >) return data_type(MPI_INT                    );
    else if constexpr (std::is_same_v<type, long                     >) return data_type(MPI_LONG                   );
    else if constexpr (std::is_same_v<type, long long                >) return data_type(MPI_LONG_LONG              );
    else if constexpr (std::is_same_v<type, signed char              >) return data_type(MPI_SIGNED_CHAR            );
    else if constexpr (std::is_same_v<type, unsigned char            >) return data_type(MPI_UNSIGNED_CHAR          );
    else if constexpr (std::is_same_v<type, unsigned short           >) return data_type(MPI_UNSIGNED_SHORT         );
    else if constexpr (std::is_same_v<type, unsigned int             >) return data_type(MPI_UNSIGNED               );
    else if constexpr (std::is_same_v<type, unsigned long            >) return data_type(MPI_UNSIGNED_LONG          );
    else if constexpr (std::is_same_v<type, unsigned long long       >) return data_type(MPI_UNSIGNED_LONG_LONG     );
    else if constexpr (std::is_same_v<type, float                    >) return data_type(MPI_FLOAT                  );
    else if constexpr (std::is_same_v<type, double                   >) return data_type(MPI_DOUBLE                 );
    else if constexpr (std::is_same_v<type, long double              >) return data_type(MPI_LONG_DOUBLE            );
    else if constexpr (std::is_same_v<type, wchar_t                  >) return data_type(MPI_WCHAR                  );
    else if constexpr (std::is_same_v<type, bool                     >) return data_type(MPI_CXX_BOOL               );
    // The following should never be visited but are included for completeness.
    else if constexpr (std::is_same_v<type, std::int8_t              >) return data_type(MPI_INT8_T                 );
    else if constexpr (std::is_same_v<type, std::int16_t             >) return data_type(MPI_INT16_T                );
    else if constexpr (std::is_same_v<type, std::int32_t             >) return data_type(MPI_INT32_T                );
    else if constexpr (std::is_same_v<type, std::int64_t             >) return data_type(MPI_INT64_T                );
    else if constexpr (std::is_same_v<type, std::uint8_t             >) return data_type(MPI_UINT8_T                );
    else if constexpr (std::is_same_v<type, std::uint16_t            >) return data_type(MPI_UINT16_T               );
    else if constexpr (std::is_same_v<type, std::uint32_t            >) return data_type(MPI_UINT32_T               );
    else if constexpr (std::is_same_v<type, std::uint64_t            >) return data_type(MPI_UINT64_T               );
    else if constexpr (std::is_same_v<type, MPI_Aint                 >) return data_type(MPI_AINT                   );
    else if constexpr (std::is_same_v<type, MPI_Count                >) return data_type(MPI_COUNT                  );
    else if constexpr (std::is_same_v<type, MPI_Offset               >) return data_type(MPI_OFFSET                 );
    //else if constexpr (std::is_same_v<type, _Bool                  >) return data_type(MPI_C_BOOL                 );
    else 
    {
      static_assert(missing_implementation<type>::value, "Missing get_data_type() implementation for arithmetic type.");
      return data_type(MPI_DATATYPE_NULL);
    }
  }
};

// Specialization for enumeration types (using standard MPI types, by forwarding the underlying arithmetic type).
template <typename type>
struct type_traits<type, std::enable_if_t<std::is_enum_v<type>>>
{
  static data_type get_data_type()
  {
    return type_traits<std::underlying_type_t<type>>::get_data_type();
  }
};

// Specialization for C-style arrays (using MPI_Type_contiguous).
template <typename type, std::size_t size>
struct type_traits<type[size]>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};
template <typename type, std::size_t size_1, std::size_t size_2>
struct type_traits<type[size_1][size_2]>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size_1 * size_2));
  }
};
template <typename type, std::size_t size_1, std::size_t size_2, std::size_t size_3>
struct type_traits<type[size_1][size_2][size_3]>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size_1 * size_2 * size_3));
  }
};
template <typename type, std::size_t size_1, std::size_t size_2, std::size_t size_3, std::size_t size_4>
struct type_traits<type[size_1][size_2][size_3][size_4]>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size_1 * size_2 * size_3 * size_4));
  }
};

// Specialization for std::arrays (using MPI_Type_contiguous).
template <typename type, std::size_t size>
struct type_traits<std::array<type, size>>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};
template <typename type, std::size_t size_1, std::size_t size_2>
struct type_traits<std::array<std::array<type, size_2>, size_1>>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size_1 * size_2));
  }
};
template <typename type, std::size_t size_1, std::size_t size_2, std::size_t size_3>
struct type_traits<std::array<std::array<std::array<type, size_3>, size_2>, size_1>>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size_1 * size_2 * size_3));
  }
};
template <typename type, std::size_t size_1, std::size_t size_2, std::size_t size_3, std::size_t size_4>
struct type_traits<std::array<std::array<std::array<std::array<type, size_4>, size_3>, size_2>, size_1>>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size_1 * size_2 * size_3 * size_4));
  }
};

// Specialization for static std::spans (using MPI_Type_contiguous).
template <typename type, std::size_t size>
struct type_traits<std::span<type, size>, std::enable_if_t<is_compliant_span_v<std::span<type, size>>>>
{
  static data_type get_data_type()
  {
    static_assert(is_compliant_v<type>, "Element does not satisfy mpi::is_compliant<type>.");
    return data_type(type_traits<type>::get_data_type(), static_cast<std::int32_t>(size));
  }
};

// Specialization for std::tuples (using MPI_Type_create_struct).
template <typename type>
struct type_traits<type, std::enable_if_t<is_tuple_v<type>>>
{
  static data_type get_data_type()
  {
    const auto count = std::tuple_size<type>::value;
  
    std::vector<data_type>    data_types   ; data_types   .reserve(count);
    std::vector<std::int32_t> block_lengths; block_lengths.reserve(count);
    std::vector<std::int64_t> displacements; displacements.reserve(count);
    std::int64_t              displacement (0);

    tuple_for_each([&] <typename lambda_type> (lambda_type& field)
    {
      static_assert(is_compliant_v<lambda_type>, "Element does not satisfy mpi::is_compliant<type>.");
      data_types   .push_back(type_traits<lambda_type>::get_data_type());
      block_lengths.push_back(1);
      displacements.push_back(displacement);
      displacement += sizeof(field);
    }, type());
  
    return data_type(data_types, block_lengths, displacements);
  }
};

// Specialization for aggregate types (using MPI_Type_create_struct).
template <typename type>
struct type_traits<type, std::enable_if_t<!std::is_arithmetic_v<type> && !std::is_enum_v<type> && !is_array_v<type> && !is_tuple_v<type> && std::is_aggregate_v<type>>>
{
  static data_type get_data_type()
  {
    const auto count = pfr::tuple_size<type>::value;
  
    std::vector<data_type>    data_types   ; data_types   .reserve(count);
    std::vector<std::int32_t> block_lengths; block_lengths.reserve(count);
    std::vector<std::int64_t> displacements; displacements.reserve(count);
    std::int64_t              displacement (0);
  
    pfr::for_each_field(type(), [&] <typename lambda_type> (lambda_type& field)
    {
      static_assert(is_compliant_v<lambda_type>, "Member does not satisfy mpi::is_compliant<type>.");
      data_types   .push_back(type_traits<lambda_type>::get_data_type());
      block_lengths.push_back(1);
      displacements.push_back(displacement);
      displacement += sizeof(field);
    });
  
    return data_type(data_types, block_lengths, displacements);
  }
};

// Specialization for std::complex (using standard MPI types).
template <>
struct type_traits<std::complex<float>>
{
  static data_type get_data_type()
  {
    return data_type(MPI_CXX_FLOAT_COMPLEX);
  }
};
template <>
struct type_traits<std::complex<double>>
{
  static data_type get_data_type()
  {
    return data_type(MPI_CXX_DOUBLE_COMPLEX);
  }
};
template <>
struct type_traits<std::complex<long double>>
{
  static data_type get_data_type()
  {
    return data_type(MPI_CXX_LONG_DOUBLE_COMPLEX);
  }
};
}