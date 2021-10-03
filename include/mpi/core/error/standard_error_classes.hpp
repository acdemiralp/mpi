#pragma once

#include <mpi/core/error/error_class.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi::error
{
inline const error_class success                                 { MPI_SUCCESS                  };
inline const error_class buffer                                  { MPI_ERR_BUFFER               };
inline const error_class count                                   { MPI_ERR_COUNT                };
inline const error_class type                                    { MPI_ERR_TYPE                 };
inline const error_class tag                                     { MPI_ERR_TAG                  };
inline const error_class communicator                            { MPI_ERR_COMM                 };
inline const error_class rank                                    { MPI_ERR_RANK                 };
inline const error_class request                                 { MPI_ERR_REQUEST              };
inline const error_class root                                    { MPI_ERR_ROOT                 };
inline const error_class group                                   { MPI_ERR_GROUP                };
inline const error_class op                                      { MPI_ERR_OP                   };
inline const error_class topology                                { MPI_ERR_TOPOLOGY             };
inline const error_class dimensions                              { MPI_ERR_DIMS                 };
inline const error_class argument                                { MPI_ERR_ARG                  };
inline const error_class unknown                                 { MPI_ERR_UNKNOWN              };
inline const error_class truncate                                { MPI_ERR_TRUNCATE             };
inline const error_class other                                   { MPI_ERR_OTHER                };
inline const error_class intern                                  { MPI_ERR_INTERN               };
inline const error_class pending                                 { MPI_ERR_PENDING              };
inline const error_class in_status                               { MPI_ERR_IN_STATUS            };
inline const error_class access                                  { MPI_ERR_ACCESS               };
inline const error_class access_mode                             { MPI_ERR_AMODE                };
inline const error_class assert                                  { MPI_ERR_ASSERT               };
inline const error_class bad_file                                { MPI_ERR_BAD_FILE             };
inline const error_class base                                    { MPI_ERR_BASE                 };
inline const error_class conversion                              { MPI_ERR_CONVERSION           };
inline const error_class displacement                            { MPI_ERR_DISP                 };
inline const error_class duplicate_data_representation           { MPI_ERR_DUP_DATAREP          };
inline const error_class file_exists                             { MPI_ERR_FILE_EXISTS          };
inline const error_class file_in_use                             { MPI_ERR_FILE_IN_USE          };
inline const error_class file                                    { MPI_ERR_FILE                 };
inline const error_class info_key                                { MPI_ERR_INFO_KEY             };
inline const error_class info_no_key                             { MPI_ERR_INFO_NOKEY           };
inline const error_class info_value                              { MPI_ERR_INFO_VALUE           };
inline const error_class info                                    { MPI_ERR_INFO                 };
inline const error_class io                                      { MPI_ERR_IO                   };
inline const error_class key_value                               { MPI_ERR_KEYVAL               };
inline const error_class lock_type                               { MPI_ERR_LOCKTYPE             };
inline const error_class name                                    { MPI_ERR_NAME                 };
inline const error_class no_memory                               { MPI_ERR_NO_MEM               };
inline const error_class not_same                                { MPI_ERR_NOT_SAME             };
inline const error_class no_space                                { MPI_ERR_NO_SPACE             };
inline const error_class no_such_file                            { MPI_ERR_NO_SUCH_FILE         };
inline const error_class port                                    { MPI_ERR_PORT                 };
inline const error_class process_aborted                         { MPI_ERR_PROC_ABORTED         };
inline const error_class quota                                   { MPI_ERR_QUOTA                };
inline const error_class read_only                               { MPI_ERR_READ_ONLY            };
inline const error_class rma_attach                              { MPI_ERR_RMA_ATTACH           };
inline const error_class rma_conflict                            { MPI_ERR_RMA_CONFLICT         };
inline const error_class rma_range                               { MPI_ERR_RMA_RANGE            };
inline const error_class rma_shared                              { MPI_ERR_RMA_SHARED           };
inline const error_class rma_sync                                { MPI_ERR_RMA_SYNC             };
inline const error_class rma_flavor                              { MPI_ERR_RMA_FLAVOR           };
inline const error_class service                                 { MPI_ERR_SERVICE              };
inline const error_class session                                 { MPI_ERR_SESSION              };
inline const error_class size                                    { MPI_ERR_SIZE                 };
inline const error_class spawn                                   { MPI_ERR_SPAWN                };
inline const error_class unsupported_data_representation         { MPI_ERR_UNSUPPORTED_DATAREP  };
inline const error_class unsupported_operation                   { MPI_ERR_UNSUPPORTED_OPERATION};
inline const error_class value_too_large                         { MPI_ERR_VALUE_TOO_LARGE      };
inline const error_class window                                  { MPI_ERR_WIN                  };
inline const error_class tool_cannot_initialize                  { MPI_T_ERR_CANNOT_INIT        };
inline const error_class tool_not_accessible                     { MPI_T_ERR_NOT_ACCESSIBLE     };
inline const error_class tool_not_initialized                    { MPI_T_ERR_NOT_INITIALIZED    };
inline const error_class tool_not_supported                      { MPI_T_ERR_NOT_SUPPORTED      };
inline const error_class tool_memory                             { MPI_T_ERR_MEMORY             };
inline const error_class invalid                                 { MPI_T_ERR_INVALID            };
inline const error_class tool_invalid_index                      { MPI_T_ERR_INVALID_INDEX      };
inline const error_class tool_invalid_item                       { MPI_T_ERR_INVALID_ITEM       };
inline const error_class tool_invalid_session                    { MPI_T_ERR_INVALID_SESSION    };
inline const error_class tool_invalid_handle                     { MPI_T_ERR_INVALID_HANDLE     };
inline const error_class invalid_name                            { MPI_T_ERR_INVALID_NAME       };
inline const error_class tool_out_of_handles                     { MPI_T_ERR_OUT_OF_HANDLES     };
inline const error_class tool_out_of_sessions                    { MPI_T_ERR_OUT_OF_SESSIONS    };
inline const error_class tool_control_variable_set_not_now       { MPI_T_ERR_CVAR_SET_NOT_NOW   };
inline const error_class tool_control_variable_set_never         { MPI_T_ERR_CVAR_SET_NEVER     };
inline const error_class tool_performance_variable_no_write      { MPI_T_ERR_PVAR_NO_WRITE      };
inline const error_class tool_performance_variable_no_start_stop { MPI_T_ERR_PVAR_NO_STARTSTOP  };
inline const error_class tool_performance_variable_no_atomic     { MPI_T_ERR_PVAR_NO_ATOMIC     };
inline const error_class last_code                               { MPI_ERR_LASTCODE             };
}