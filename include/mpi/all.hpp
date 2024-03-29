#pragma once

#include <mpi/core/mpi.hpp>
#include <mpi/core/communicators/cartesian_communicator.hpp>
#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/communicators/distributed_graph_communicator.hpp>
#include <mpi/core/communicators/graph_communicator.hpp>
#include <mpi/core/enums/combiner.hpp>
#include <mpi/core/enums/comparison.hpp>
#include <mpi/core/enums/distribution.hpp>
#include <mpi/core/enums/mode.hpp>
#include <mpi/core/enums/profiling_level.hpp>
#include <mpi/core/enums/split_type.hpp>
#include <mpi/core/enums/thread_support.hpp>
#include <mpi/core/enums/topology.hpp>
#include <mpi/core/enums/type_class.hpp>
#include <mpi/core/enums/window_flavor.hpp>
#include <mpi/core/error/error_class.hpp>
#include <mpi/core/error/error_code.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/error/standard_error_classes.hpp>
#include <mpi/core/structs/data_type_information.hpp>
#include <mpi/core/structs/dimension.hpp>
#include <mpi/core/structs/distributed_array_information.hpp>
#include <mpi/core/structs/distributed_graph.hpp>
#include <mpi/core/structs/graph.hpp>
#include <mpi/core/structs/neighbor_counts.hpp>
#include <mpi/core/structs/neighbor_information.hpp>
#include <mpi/core/structs/overhead_type.hpp>
#include <mpi/core/structs/process_set.hpp>
#include <mpi/core/structs/range.hpp>
#include <mpi/core/structs/reduction_types.hpp>
#include <mpi/core/structs/spawn_information.hpp>
#include <mpi/core/structs/sub_array_information.hpp>
#include <mpi/core/structs/window_information.hpp>
#include <mpi/core/type/compliant_container_traits.hpp>
#include <mpi/core/type/compliant_traits.hpp>
#include <mpi/core/type/data_type.hpp>
#include <mpi/core/type/data_type_traits.hpp>
#include <mpi/core/type/standard_data_types.hpp>
#include <mpi/core/type/type_traits.hpp>
#include <mpi/core/utility/array_traits.hpp>
#include <mpi/core/utility/associative_container_traits.hpp>
#include <mpi/core/utility/bitset_enum.hpp>
#include <mpi/core/utility/complex_traits.hpp>
#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/utility/container_traits.hpp>
#include <mpi/core/utility/contiguous.hpp>
#include <mpi/core/utility/missing_implementation.hpp>
#include <mpi/core/utility/sequential_container_traits.hpp>
#include <mpi/core/utility/span_traits.hpp>
#include <mpi/core/utility/tuple_traits.hpp>
#include <mpi/core/environment.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/generalized_request.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/key_value.hpp>
#include <mpi/core/memory.hpp>
#include <mpi/core/message.hpp>
#include <mpi/core/op.hpp>
#include <mpi/core/port.hpp>
#include <mpi/core/request.hpp>
#include <mpi/core/service.hpp>
#include <mpi/core/session.hpp>
#include <mpi/core/standard_ops.hpp>
#include <mpi/core/status.hpp>
#include <mpi/core/time.hpp>
#include <mpi/core/version.hpp>
#include <mpi/core/window.hpp>

#include <mpi/extensions/detach.hpp>
#include <mpi/extensions/future.hpp>
#include <mpi/extensions/shared_variable.hpp>

#include <mpi/io/enums/access_mode.hpp>
#include <mpi/io/enums/seek_mode.hpp>
#include <mpi/io/structs/file_view.hpp>
#include <mpi/io/data_representation.hpp>
#include <mpi/io/file.hpp>

#include <mpi/tool/enums/bind_type.hpp>
#include <mpi/tool/enums/callback_safety.hpp>
#include <mpi/tool/enums/performance_variable_type.hpp>
#include <mpi/tool/enums/scope.hpp>
#include <mpi/tool/enums/verbosity.hpp>
#include <mpi/tool/structs/category.hpp>
#include <mpi/tool/structs/control_variable.hpp>
#include <mpi/tool/structs/enumeration.hpp>
#include <mpi/tool/structs/enumeration_item.hpp>
#include <mpi/tool/structs/event.hpp>
#include <mpi/tool/structs/event_source.hpp>
#include <mpi/tool/structs/performance_variable.hpp>
#include <mpi/tool/utility/object_variant.hpp>
#include <mpi/tool/control_variable_handle.hpp>
#include <mpi/tool/environment.hpp>
#include <mpi/tool/event_handle.hpp>
#include <mpi/tool/event_instance.hpp>
#include <mpi/tool/performance_variable_handle.hpp>
#include <mpi/tool/session.hpp>