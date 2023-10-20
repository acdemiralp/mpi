#pragma once

#include <cstdint>

#include <mpi/core/mpi.hpp>

namespace mpi::tool
{
enum class bind_type : std::int32_t
{
  none          = MPI_T_BIND_NO_OBJECT     ,
  communicator  = MPI_T_BIND_MPI_COMM      ,
  data_type     = MPI_T_BIND_MPI_DATATYPE  ,
  error_handler = MPI_T_BIND_MPI_ERRHANDLER,
  file          = MPI_T_BIND_MPI_FILE      ,
  group         = MPI_T_BIND_MPI_GROUP     ,
  op            = MPI_T_BIND_MPI_OP        ,
  request       = MPI_T_BIND_MPI_REQUEST   ,
  window        = MPI_T_BIND_MPI_WIN       ,
  message       = MPI_T_BIND_MPI_MESSAGE   ,
  information   = MPI_T_BIND_MPI_INFO
#ifdef MPI_GEQ_4_1
                                           , 
  session       = MPI_T_BIND_MPI_SESSION
#endif
};
}

