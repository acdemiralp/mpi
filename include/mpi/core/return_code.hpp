#pragma once

#include <mpi/core/mpi.hpp>

namespace mpi
{
enum class return_code
{
  success                                 = MPI_SUCCESS                  ,
  buffer                                  = MPI_ERR_BUFFER               ,
  count                                   = MPI_ERR_COUNT                ,
  type                                    = MPI_ERR_TYPE                 ,
  tag                                     = MPI_ERR_TAG                  ,
  communicator                            = MPI_ERR_COMM                 ,
  rank                                    = MPI_ERR_RANK                 ,
  request                                 = MPI_ERR_REQUEST              ,
  root                                    = MPI_ERR_ROOT                 ,
  group                                   = MPI_ERR_GROUP                ,
  op                                      = MPI_ERR_OP                   ,
  topology                                = MPI_ERR_TOPOLOGY             ,
  dimensions                              = MPI_ERR_DIMS                 ,
  argument                                = MPI_ERR_ARG                  ,
  unknown                                 = MPI_ERR_UNKNOWN              ,
  truncate                                = MPI_ERR_TRUNCATE             ,
  other                                   = MPI_ERR_OTHER                ,
  intern                                  = MPI_ERR_INTERN               ,
  in_status                               = MPI_ERR_IN_STATUS            ,
  pending                                 = MPI_ERR_PENDING              ,
  access                                  = MPI_ERR_ACCESS               ,
  amode                                   = MPI_ERR_AMODE                ,
  assert                                  = MPI_ERR_ASSERT               ,
  bad_file                                = MPI_ERR_BAD_FILE             ,
  base                                    = MPI_ERR_BASE                 ,
  conversion                              = MPI_ERR_CONVERSION           ,
  displacement                            = MPI_ERR_DISP                 ,
  duplicate_data_representation           = MPI_ERR_DUP_DATAREP          ,
  file_exists                             = MPI_ERR_FILE_EXISTS          ,
  file_in_use                             = MPI_ERR_FILE_IN_USE          ,
  file                                    = MPI_ERR_FILE                 ,
  info_key                                = MPI_ERR_INFO_KEY             ,
  info_no_key                             = MPI_ERR_INFO_NOKEY           ,
  info_value                              = MPI_ERR_INFO_VALUE           ,
  info                                    = MPI_ERR_INFO                 ,
  io                                      = MPI_ERR_IO                   ,
  key_value                               = MPI_ERR_KEYVAL               ,
  lock_type                               = MPI_ERR_LOCKTYPE             ,
  name                                    = MPI_ERR_NAME                 ,
  no_memory                               = MPI_ERR_NO_MEM               ,
  not_same                                = MPI_ERR_NOT_SAME             ,
  no_space                                = MPI_ERR_NO_SPACE             ,
  no_such_file                            = MPI_ERR_NO_SUCH_FILE         ,
  port                                    = MPI_ERR_PORT                 ,
  quota                                   = MPI_ERR_QUOTA                ,
  read_only                               = MPI_ERR_READ_ONLY            ,
  rma_conflict                            = MPI_ERR_RMA_CONFLICT         ,
  rma_sync                                = MPI_ERR_RMA_SYNC             ,
  service                                 = MPI_ERR_SERVICE              ,
  size                                    = MPI_ERR_SIZE                 ,
  spawn                                   = MPI_ERR_SPAWN                ,
  unsupported_data_representation         = MPI_ERR_UNSUPPORTED_DATAREP  ,
  unsupported_operation                   = MPI_ERR_UNSUPPORTED_OPERATION,
  window                                  = MPI_ERR_WIN                  ,
  tool_memory                             = MPI_T_ERR_MEMORY             ,
  tool_not_initialized                    = MPI_T_ERR_NOT_INITIALIZED    ,
  tool_cannot_initialize                  = MPI_T_ERR_CANNOT_INIT        ,
  tool_invalid_index                      = MPI_T_ERR_INVALID_INDEX      ,
  tool_invalid_item                       = MPI_T_ERR_INVALID_ITEM       ,
  tool_invalid_handle                     = MPI_T_ERR_INVALID_HANDLE     ,
  tool_out_of_handles                     = MPI_T_ERR_OUT_OF_HANDLES     ,
  tool_out_of_sessions                    = MPI_T_ERR_OUT_OF_SESSIONS    ,
  tool_invalid_session                    = MPI_T_ERR_INVALID_SESSION    ,
  tool_control_variable_set_not_now       = MPI_T_ERR_CVAR_SET_NOT_NOW   ,
  tool_control_variable_set_never         = MPI_T_ERR_CVAR_SET_NEVER     ,
  tool_performance_variable_no_start_stop = MPI_T_ERR_PVAR_NO_STARTSTOP  ,
  tool_performance_variable_no_write      = MPI_T_ERR_PVAR_NO_WRITE      ,
  tool_performance_variable_no_atomic     = MPI_T_ERR_PVAR_NO_ATOMIC     ,
  rma_range                               = MPI_ERR_RMA_RANGE            ,
  rma_attach                              = MPI_ERR_RMA_ATTACH           ,
  rma_flavor                              = MPI_ERR_RMA_FLAVOR           ,
  rma_shared                              = MPI_ERR_RMA_SHARED           ,
  invalid                                 = MPI_T_ERR_INVALID            ,
  invalid_name                            = MPI_T_ERR_INVALID_NAME       ,
  last_code                               = MPI_ERR_LASTCODE
};
}