#pragma once

#include <mpi/core/data_type.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::standard_data_types
{
inline const data_type char_                   = data_type(MPI_CHAR                   );
inline const data_type short_                  = data_type(MPI_SHORT                  );
inline const data_type int_                    = data_type(MPI_INT                    );
inline const data_type long_                   = data_type(MPI_LONG                   );
inline const data_type long_long               = data_type(MPI_LONG_LONG              );
inline const data_type signed_char             = data_type(MPI_SIGNED_CHAR            );
inline const data_type unsigned_char           = data_type(MPI_UNSIGNED_CHAR          );
inline const data_type unsigned_short          = data_type(MPI_UNSIGNED_SHORT         );
inline const data_type unsigned_int            = data_type(MPI_UNSIGNED               );
inline const data_type unsigned_long           = data_type(MPI_UNSIGNED_LONG          );
inline const data_type unsigned_long_long      = data_type(MPI_UNSIGNED_LONG_LONG     );
inline const data_type float_                  = data_type(MPI_FLOAT                  );
inline const data_type double_                 = data_type(MPI_DOUBLE                 );
inline const data_type long_double             = data_type(MPI_LONG_DOUBLE            );
inline const data_type wchar                   = data_type(MPI_WCHAR                  );
inline const data_type c_bool                  = data_type(MPI_C_BOOL                 );
inline const data_type int8_t                  = data_type(MPI_INT8_T                 );
inline const data_type int16_t                 = data_type(MPI_INT16_T                );
inline const data_type int32_t                 = data_type(MPI_INT32_T                );
inline const data_type int64_t                 = data_type(MPI_INT64_T                );
inline const data_type uint8_t                 = data_type(MPI_UINT8_T                );
inline const data_type uint16_t                = data_type(MPI_UINT16_T               );
inline const data_type uint32_t                = data_type(MPI_UINT32_T               );
inline const data_type uint64_t                = data_type(MPI_UINT64_T               );
inline const data_type aint                    = data_type(MPI_AINT                   );
inline const data_type count                   = data_type(MPI_COUNT                  );
inline const data_type offset                  = data_type(MPI_OFFSET                 );
inline const data_type c_complex               = data_type(MPI_C_COMPLEX              );
inline const data_type c_float_complex         = data_type(MPI_C_FLOAT_COMPLEX        );
inline const data_type c_double_complex        = data_type(MPI_C_DOUBLE_COMPLEX       );
inline const data_type c_long_double_complex   = data_type(MPI_C_LONG_DOUBLE_COMPLEX  );
inline const data_type byte                    = data_type(MPI_BYTE                   );
inline const data_type packed                  = data_type(MPI_PACKED                 );
inline const data_type bool_                   = data_type(MPI_CXX_BOOL               );
inline const data_type std_complex_float       = data_type(MPI_CXX_FLOAT_COMPLEX      );
inline const data_type std_complex_double      = data_type(MPI_CXX_DOUBLE_COMPLEX     );
inline const data_type std_complex_long_double = data_type(MPI_CXX_LONG_DOUBLE_COMPLEX);

namespace reduction
{
inline const data_type float_int               = data_type(MPI_FLOAT_INT              );
inline const data_type double_int              = data_type(MPI_DOUBLE_INT             );
inline const data_type long_int                = data_type(MPI_LONG_INT               );
inline const data_type two_int                 = data_type(MPI_2INT                   );
inline const data_type short_int               = data_type(MPI_SHORT_INT              );
inline const data_type long_double_int         = data_type(MPI_LONG_DOUBLE_INT        );
}
}