#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/enums/comparison.hpp>
#include <mpi/core/enums/split_type.hpp>
#include <mpi/core/enums/topology.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/structs/spawn_information.hpp>
#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/key_value.hpp>
#include <mpi/core/message.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/op.hpp>
#include <mpi/core/port.hpp>
#include <mpi/core/standard_ops.hpp>
#include <mpi/core/request.hpp>

namespace mpi
{
class communicator
{
public:
  explicit communicator   (const MPI_Comm       native, const bool managed = false)
  : managed_(managed), native_(native)
  {
    
  }
  communicator            (const communicator&  that , const group&                          group        )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create       , (that.native_, group      .native(),      &native_))
  }
  communicator            (const communicator&  that , const group&                          group        , const std::int32_t  tag)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_group , (that.native_, group      .native(), tag, &native_))
  }
#ifdef MPI_USE_LATEST
  communicator            (                            const group&                          group        , const std::string&  tag      , const information& information, const communicator_error_handler& error_handler)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_from_group, (group.native(), tag.c_str(), information.native(), error_handler.native(), &native_))
  }
#endif
  communicator            (const communicator&  that , const std::int32_t                    color        , const std::int32_t  key)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_split        , (that.native_, color, key, &native_))
  }
  communicator            (const communicator&  that , const split_type                      split_type   , const std::int32_t  key   = 0, const information& information       = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_split_type   , (that.native_, static_cast<std::int32_t>(split_type), key, information.native(), &native_))
  }
  communicator            (const communicator&  that , const port&                           port         , const bool          accept   , const information& information       = mpi::information(), const std::int32_t root = 0)
  : managed_(true)
  {
    if (accept)
      MPI_CHECK_ERROR_CODE(MPI_Comm_accept , (port.name().c_str(), information.native(), root, that.native(), &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Comm_connect, (port.name().c_str(), information.native(), root, that.native(), &native_))
  }
  communicator            (const communicator&  that , const spawn_information&              spawn_info   , const std::int32_t  root  = 0, const bool         check_error_codes = true)
  : managed_(true)
  {
    std::vector<char*> arguments(spawn_info.arguments.size());
    std::ranges::transform(spawn_info.arguments, arguments.begin(), [ ] (const std::string& value)
    {
      return const_cast<char*>(value.data());
    });

    std::vector<std::int32_t> error_codes(spawn_info.process_count);

    MPI_CHECK_ERROR_CODE(MPI_Comm_spawn, (spawn_info.command.c_str(), arguments.empty() ? MPI_ARGV_NULL : arguments.data(), spawn_info.process_count, spawn_info.information.native(), root, that.native(), &native_, check_error_codes ? error_codes.data() : MPI_ERRCODES_IGNORE))

    if (check_error_codes)
      for (const auto& code : error_codes)
        if (code != MPI_SUCCESS)
          throw exception("spawn", error_code(code));
  }
  communicator            (const communicator&  that , const std::vector<spawn_information>& spawn_info   , const std::int32_t  root  = 0, const bool         check_error_codes = true)
  : managed_(true)
  {
    std::vector<char*>              commands      (spawn_info.size());
    std::vector<std::vector<char*>> arguments     (spawn_info.size());
    std::vector<char**>             raw_arguments (spawn_info.size());
    std::vector<std::int32_t>       process_counts(spawn_info.size());
    std::vector<MPI_Info>           infos         (spawn_info.size());
    std::int32_t                    total_count   (0);
    for (std::size_t i = 0; i < spawn_info.size(); ++i)
    {
      arguments[i].resize(spawn_info[i].arguments.size());
      std::ranges::transform(spawn_info[i].arguments, arguments[i].begin(), [ ] (const std::string& value)
      {
        return const_cast<char*>(value.data());
      });

      commands      [i]  = const_cast<char*>(spawn_info[i].command.c_str());
      raw_arguments [i]  = arguments [i].empty() ? MPI_ARGV_NULL : arguments[i].data();
      process_counts[i]  = spawn_info[i].process_count;
      infos         [i]  = spawn_info[i].information.native();
      total_count       += spawn_info[i].process_count;
    }

    std::vector<std::int32_t> error_codes(total_count);

    MPI_CHECK_ERROR_CODE(MPI_Comm_spawn_multiple, (static_cast<std::int32_t>(spawn_info.size()), commands.data(), raw_arguments.data(), process_counts.data(), infos.data(), root, that.native(), &native_, check_error_codes ? error_codes.data() : MPI_ERRCODES_IGNORE))

    if (check_error_codes)
      for (const auto& code : error_codes)
        if (code != MPI_SUCCESS)
          throw exception("spawn_multiple", error_code(code));
  }
  communicator            (const communicator&  local, const std::int32_t                    local_leader , const communicator& peer     , const std::int32_t peer_leader, const std::int32_t tag = 0)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Intercomm_create, (local.native_, local_leader, peer.native_, peer_leader, tag, &native_))
  }
#ifdef MPI_USE_LATEST
  communicator            (const group&         local, const std::int32_t                    local_leader , const group&        peer     , const std::int32_t peer_leader, const std::string& tag, const information& information, const communicator_error_handler& error_handler)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Intercomm_create_from_groups, (local.native_, local_leader, peer.native_, peer_leader, tag.c_str(), information.native(), error_handler.native(), &native_))
  }
#endif
  communicator            (const communicator&  that , const bool                            high         )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Intercomm_merge, (that.native_, high, &native_))
  }
  communicator            (const communicator&  that , const information&                    information  )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_dup_with_info, (that.native_, information.native(), &native_))
  }
  communicator            (const communicator&  that)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_dup, (that.native_, &native_))
  }
  communicator            (      communicator&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_COMM_NULL;
  }
  virtual ~communicator   () noexcept(false)
  {
    if (managed_ && native_ != MPI_COMM_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Comm_free, (&native_))
  }
  communicator& operator= (const communicator&  that)
  {
    if (this != &that)
    {
      if (managed_ && native_ != MPI_COMM_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Comm_free, (&native_))

      managed_ = true;
      MPI_CHECK_ERROR_CODE(MPI_Comm_dup, (that.native_, &native_))
    }
    return *this;
  }
  communicator& operator= (      communicator&& temp) noexcept(false)
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_COMM_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Comm_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_COMM_NULL;
    }
    return *this;
  }

  bool          operator==(const communicator&  that) const
  {
    return compare(that) == comparison::identical;
  }

  // A static member function for construction is bad practice but the alternative is to use the default constructor for retrieving the parent communicator, which is unclear transmission of intent.
  [[nodiscard]]
  static communicator                       parent                        ()
  {
    communicator result(MPI_COMM_NULL, false); // Unmanaged.
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_parent, (&result.native_))
    return result;
  }
  // A static member function for construction is bad practice but the unmanaged constructor also takes a single std::int32_t (an MPI_Comm). MPI should consider using unique structs rather than integers for handles.
  [[nodiscard]]
  static communicator                       join                          (const std::int32_t socket_file_descriptor)
  {
    communicator result(MPI_COMM_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Comm_join, (socket_file_descriptor, &result.native_))
    return result;
  }
                                                                          
  [[nodiscard]]                                                           
  std::int32_t                              rank                          () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_rank, (native_, &result))
    return result;
  }
  [[nodiscard]]                                                           
  std::int32_t                              size                          () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_size, (native_, &result))
    return result;
  }
                                                                          
  [[nodiscard]]                                                           
  group                                     group                         () const
  {
    mpi::group result(MPI_GROUP_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Comm_group, (native_, &result.native_))
    return result;
  }
  [[nodiscard]]                                                           
  topology                                  topology                      () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Topo_test, (native_, &result))
    return static_cast<mpi::topology>(result);
  }
                                                                          
  [[nodiscard]]                                                           
  bool                                      is_intercommunicator          () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_test_inter, (native_, &result))
    return static_cast<bool>(result);
  }
  [[nodiscard]]                                                           
  mpi::group                                remote_group                  () const
  {
    mpi::group result(MPI_GROUP_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Comm_remote_group, (native_, &result.native_))
    return result;
  }
  [[nodiscard]]                                                           
  std::int32_t                              remote_size                   () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_remote_size, (native_, &result))
    return result;
  }
                                                                          
  [[nodiscard]]                                                           
  comparison                                compare                       (const communicator& that) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_compare, (native_, that.native_, &result))
    return static_cast<comparison>(result);
  }
                                                                          
  [[nodiscard]]                                                           
  std::string                               name                          () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, '\n');
    std::int32_t length(0);
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_name, (native_, &result[0], &length))
    result.resize(static_cast<std::size_t>(length));
    return result;
  }
  void                                      set_name                      (const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_name, (native_, value.c_str()))
  }
                                                                          
  [[nodiscard]]                                                           
  information                               information                   () const
  {
    mpi::information result(MPI_INFO_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_info, (native_, &result.native_))
    return result;
  }
  void                                      set_information               (const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_info, (native_, value.native()))
  }
                                                                          
  [[nodiscard]]                                                           
  communicator_error_handler                error_handler                 () const
  {
    communicator_error_handler result(MPI_ERRHANDLER_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_errhandler, (native_, &result.native_))
    return result;
  }
  void                                      set_error_handler             (const communicator_error_handler& value)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_errhandler, (native_, value.native()))
  }
  void                                      call_error_handler            (const error_code& value)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_call_errhandler, (native_, value.native()))
  }
                                                                          
  template <typename type> [[nodiscard]]                                              
  std::optional<type>                       attribute                     (const communicator_key_value& key) const
  {
    type*        result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_attr   , (native_, key.native(), static_cast<void*>(&result), &exists))
    return static_cast<bool>(exists) ? *result : std::optional<type>(std::nullopt);
  }
  template <typename type>                                                
  void                                      set_attribute                 (const communicator_key_value& key, const type& value)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_attr   , (native_, key.native(), static_cast<void*>(&value)))
  }
  void                                      remove_attribute              (const communicator_key_value& key)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_delete_attr, (native_, key.native()))
  }

  void                                      abort                         (const error_code& error_code) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Abort, (native_, error_code.native()))
  }
  // Disconnect must currently be called explicitly. It is possible to state keep whether the accept/connect constructor has been used and call disconnect in the destructor if so.
  void                                      disconnect                    ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_disconnect, (&native_))
  }
                                                                          
  [[nodiscard]]                                                           
  std::pair<communicator, request>          immediate_duplicate           () const
  {
    std::pair result { communicator(MPI_COMM_NULL, true), request(MPI_REQUEST_NULL, true) };
    MPI_CHECK_ERROR_CODE(MPI_Comm_idup, (native_, &result.first.native_, &result.second.native_))
    return result;
  }
#ifdef MPI_USE_LATEST                                                                 
  [[nodiscard]]                                                       
  std::pair<communicator, request>          immediate_duplicate           (const std::information& information) const
  {
    std::pair result { communicator(), request() };
    MPI_CHECK_ERROR_CODE(MPI_Comm_idup_with_info, (native_, information.native(), &result.first.native_, &result.second.native_))
    return result;
  }
#endif

  // Pack / unpack operations.
  [[nodiscard]]
  std::int32_t                              pack_size                     (const std::int32_t count, const data_type& data_type) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Pack_size, (count, data_type.native(), native_, &result))
    return result;
  }
  [[nodiscard]]
  std::int32_t                              pack                          (const void*       input, const std::int32_t input_size, const data_type&   input_data_type, void*              output, const std::int32_t output_size, const std::int32_t output_position ) const
  {
    std::int32_t result(output_position);
    MPI_CHECK_ERROR_CODE(MPI_Pack, (input, input_size, input_data_type.native(), output, output_size, &result, native_))
    return result;
  }
  template <typename input_type, typename output_type> [[nodiscard]]
  std::int32_t                              pack                          (const input_type& input,                                                                    const output_type& output                                , const std::int32_t output_position ) const
  {
    using input_adapter  = container_adapter<input_type >;
    using output_adapter = container_adapter<output_type>;

    return pack(input_adapter::data(input), static_cast<std::int32_t>(input_adapter::size(input)), input_adapter::data_type(), output_adapter::data(output), static_cast<std::int32_t>(output_adapter::size(output)), output_position);
  }
  [[nodiscard]]
  std::int32_t                              unpack                        (const void*       input, const std::int32_t input_size, const std::int32_t input_position , void*              output, const std::int32_t output_size, const data_type&   output_data_type) const
  {
    std::int32_t result(input_position);
    MPI_CHECK_ERROR_CODE(MPI_Unpack, (input, input_size, &result, output, output_size, output_data_type.native(), native_))
    return result;
  }
  template <typename input_type, typename output_type> [[nodiscard]]
  std::int32_t                              unpack                        (const input_type& input,                                                                    const output_type& output                                , const std::int32_t input_position  ) const
  {
    using input_adapter  = container_adapter<input_type >;
    using output_adapter = container_adapter<output_type>;

    return unpack(input_adapter::data(input), static_cast<std::int32_t>(input_adapter::size(input)), input_position, output_adapter::data(output), static_cast<std::int32_t>(output_adapter::size(output)), output_adapter::data_type());
  }

  // Point-to-point operations.                                   
  void                                      send                          (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Send, (data, size, data_type.native(), destination, tag, native_))
  }
  template <typename type>                                                
  void                                      send                          (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }
                                                                            
  void                                      synchronous_send              (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Ssend, (data, size, data_type.native(), destination, tag, native_))
  }
  template <typename type>                                                
  void                                      synchronous_send              (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    synchronous_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }
                                                                            
  void                                      buffered_send                 (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Bsend, (data, size, data_type.native(), destination, tag, native_))
  }
  template <typename type>                                                
  void                                      buffered_send                 (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    buffered_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }
                                                               
  void                                      ready_send                    (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Rsend, (data, size, data_type.native(), destination, tag, native_))
  }
  template <typename type>                                                
  void                                      ready_send                    (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    ready_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }

  [[nodiscard]]                                                           
  request                                   immediate_send                (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Isend, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_send                (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }
  [[nodiscard]]
  request                                   immediate_synchronous_send    (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Issend, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]          
  request                                   immediate_synchronous_send    (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_synchronous_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }                                                          
  [[nodiscard]]
  request                                   immediate_buffered_send       (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ibsend, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]          
  request                                   immediate_buffered_send       (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_buffered_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }                                                          
  [[nodiscard]]                                                           
  request                                   immediate_ready_send          (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Irsend, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_ready_send          (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_ready_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }

  [[nodiscard]]                                                           
  request                                   persistent_send               (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Send_init, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_send               (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return persistent_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }
  [[nodiscard]]                                                           
  request                                   persistent_synchronous_send   (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Ssend_init, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_synchronous_send   (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return persistent_synchronous_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }
  [[nodiscard]]
  request                                   persistent_buffered_send      (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Bsend_init, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]          
  request                                   persistent_buffered_send      (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return persistent_buffered_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }                                                          
  [[nodiscard]]                                                           
  request                                   persistent_ready_send         (const void* data, const std::int32_t size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Rsend_init, (data, size, data_type.native(), destination, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_ready_send         (const type& data,                                                      const std::int32_t destination, const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return persistent_ready_send(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, tag);
  }

#ifdef MPI_USE_LATEST 
  [[nodiscard]]                                                           
  request                                   partitioned_send              (const std::int32_t partitions, const void* data, const count size, const data_type& data_type, const std::int32_t destination, const std::int32_t tag = 0, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Psend_init, (data, partitions, size, data_type.native(), destination, tag, native_, info.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   partitioned_send              (const std::int32_t partitions, const type& data,                                               const std::int32_t destination, const std::int32_t tag = 0, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return partitioned_send(partitions, static_cast<const void*>(adapter::data(data)), static_cast<count>(adapter::size(data)), adapter::data_type(), destination, tag, info);
  }
#endif
  
  status                                    receive                       (      void* data, const std::int32_t size, const data_type& data_type, const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_Recv, (data, size, data_type.native(), source, tag, native_, &result))
    return result;
  }
  template <typename type>
  status                                    receive                       (      type& data,                                                      const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    using adapter = container_adapter<type>;
    return receive(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), source, tag);
  }
  [[nodiscard]]
  request                                   immediate_receive             (      void* data, const std::int32_t size, const data_type& data_type, const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Irecv, (data, size, data_type.native(), source, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_receive             (      type& data,                                                      const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    using adapter = container_adapter<type>;
    return immediate_receive(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), source, tag);
  }
  [[nodiscard]]
  request                                   persistent_receive            (      void* data, const std::int32_t size, const data_type& data_type, const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Recv_init, (data, size, data_type.native(), source, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_receive            (      type& data,                                                      const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    using adapter = container_adapter<type>;
    return persistent_receive(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), source, tag);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   partitioned_receive           (const std::int32_t partitions, void* data, const count size, const data_type& data_type, const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Precv_init, (data, partitions, size, data_type.native(), source, tag, native_, info.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request                                   partitioned_receive           (const std::int32_t partitions, type& data,                                               const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return partitioned_receive(partitions, static_cast<void*>(adapter::data(data)), static_cast<count>(adapter::size(data)), adapter::data_type(), source, tag, info);
  }
#endif

  status                                    send_receive                  (const void*         sent       , const std::int32_t sent_size    , const data_type&   sent_data_type    , const std::int32_t destination                 , const std::int32_t send_tag    ,
                                                                                 void*         received   , const std::int32_t received_size, const data_type&   received_data_type, const std::int32_t source      = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_Sendrecv, (sent    , sent_size    , sent_data_type    .native(), destination, send_tag   ,
                                        received, received_size, received_data_type.native(), source     , receive_tag, native_, &result))
    return result;
  }
  template <typename sent_type, typename received_type>                                                 
  status                                    send_receive                  (const sent_type&     sent      , const std::int32_t destination  ,                 const std::int32_t send_tag    ,
                                                                                 received_type& received  , const std::int32_t source       = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    using send_adapter    = container_adapter<sent_type    >;
    using receive_adapter = container_adapter<received_type>;
    return send_receive(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter   ::size(sent    )), send_adapter   ::data_type(), destination, send_tag   , 
      static_cast<      void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), source     , receive_tag);
  }                                                                                                                                                             
  status                                    send_receive_replace          (      void*         data       , const std::int32_t size         , const data_type&   data_type                 , 
                                                                           const std::int32_t  destination, const std::int32_t send_tag     , const std::int32_t source    = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_Sendrecv_replace, (data, size, data_type.native(), destination, send_tag, source, receive_tag, native_, &result))
    return result;
  }
  template <typename type>                                                                                                                                          
  status                                    send_receive_replace          (const type&         data       , const std::int32_t destination  , const std::int32_t send_tag, const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    using adapter = container_adapter<type>;
    return send_receive_replace(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, send_tag, source, receive_tag);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   immediate_send_receive        (const void*         sent       , const std::int32_t sent_size    , const data_type&   sent_data_type    , const std::int32_t destination                 , const std::int32_t send_tag    , 
                                                                                 void*         received   , const std::int32_t received_size, const data_type&   received_data_type, const std::int32_t source      = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    request result;
    MPI_CHECK_ERROR_CODE(MPI_Isendrecv, (sent    , sent_size    , sent_data_type    .native(), destination, send_tag   ,
                                         received, received_size, received_data_type.native(), source     , receive_tag, native_, &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_send_receive        (const sent_type&     sent      , const std::int32_t destination                  , const std::int32_t send_tag    , 
                                                                                 received_type& received  , const std::int32_t source       = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    using send_adapter    = container_adapter<sent_type    >;
    using receive_adapter = container_adapter<received_type>;
    return immediate_send_receive(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter   ::size(sent    )), send_adapter   ::data_type(), destination, send_tag    , 
      static_cast<      void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), source     , receive_tag);
  }
  [[nodiscard]]                                                                                                                                                                         
  request                                   immediate_send_receive_replace(      void*         data       , const std::int32_t size         , const data_type&   data_type , 
                                                                           const std::int32_t  destination, const std::int32_t send_tag     , const std::int32_t source    = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    request result;
    MPI_CHECK_ERROR_CODE(MPI_Isendrecv_replace, (data, size, data_type.native(), destination, send_tag, source, receive_tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                                                                                                                
  request                                   immediate_send_receive_replace(const type&         data       , const std::int32_t  destination , const std::int32_t send_tag, const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t receive_tag = MPI_ANY_TAG) const
  {
    using adapter = container_adapter<type>;
    return immediate_send_receive_replace(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, send_tag, source, receive_tag);
  }
#endif 

  [[nodiscard]]                                                       
  status                                    probe                         (const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_Probe, (source, tag, native_, &result))
    return result;
  }
  [[nodiscard]]                                                       
  std::pair<message, status>                probe_message                 (const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    message    message(MPI_MESSAGE_NULL);
    MPI_Status status;
    MPI_CHECK_ERROR_CODE(MPI_Mprobe , (source, tag, native_, &message.native_, &status))
    return {message, status};
  }
  [[nodiscard]]                                                           
  std::optional<status>                     immediate_probe               (const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    MPI_Status   result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Iprobe, (source, tag, native_, &exists, &result))
    return static_cast<bool>(exists) ? result : std::optional<status>(std::nullopt);
  }
  [[nodiscard]]                                                           
  std::optional<std::pair<message, status>> immediate_probe_message       (const std::int32_t source = MPI_ANY_SOURCE, const std::int32_t tag = MPI_ANY_TAG) const
  {
    message      message(MPI_MESSAGE_NULL);
    MPI_Status   status;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Improbe, (source, tag, native_, &exists, &message.native_, &status))
    return static_cast<bool>(exists) ? std::pair<mpi::message, mpi::status>{message, status} : std::optional<std::pair<mpi::message, mpi::status>>(std::nullopt);
  }

  // All to all collective operations.

  void                                      barrier                       () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Barrier, (native_))
  }
  [[nodiscard]]                                                           
  request                                   immediate_barrier             () const
  {
    request request(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ibarrier, (native_, &request.native_))
    return request;
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_barrier            (const mpi::information& info = mpi::information()) const
  {
    request request(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Barrier_init, (native_, info.native(), &request.native_))
    return request;
  }
#endif

  void                                      all_to_all                    (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Alltoall, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_to_all                    (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    all_to_all(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
  template <typename type>                            
  void                                      all_to_all                    (type& data) const
  {
    using adapter = container_adapter<type>;
    all_to_all(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]                                                           
  request                                   immediate_all_to_all          (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ialltoall, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_all_to_all          (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_all_to_all(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_all_to_all          (type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_all_to_all(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_all_to_all         (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Alltoall_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   persistent_all_to_all         (sent_type& sent_data, received_type& received_data, const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_all_to_all(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), info);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_all_to_all         (type& data, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_all_to_all(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), info);
  }
#endif

  void                                      all_to_all_varying            (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type& sent_data_type    ,
                                                                                 void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Alltoallv, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                         received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_to_all_varying            (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                                                 received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_to_all_varying            (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , 
                                                                                 received_type& received, const std::vector<std::int32_t>& received_sizes, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    
    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> sent_displacements    (sent_sizes    .size());
    std::vector<std::int32_t> received_displacements(received_sizes.size());
    std::exclusive_scan(sent_sizes    .begin(), sent_sizes    .end(), sent_displacements    .begin(), 0);
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), received_displacements.begin(), 0);

    all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_to_all_varying            (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , 
                                                                                 received_type& received                                                 , const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    std::vector<std::int32_t> received_sizes(sent_sizes.size());
    all_to_all(sent_sizes, received_sizes);

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> sent_displacements    (sent_sizes    .size());
    std::vector<std::int32_t> received_displacements(received_sizes.size());
    std::exclusive_scan(sent_sizes    .begin(), sent_sizes    .end(), sent_displacements    .begin(), 0);
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), received_displacements.begin(), 0);

    all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
  template <typename type>                            
  void                                      all_to_all_varying            (      type&          data    , const std::vector<std::int32_t>& sizes         , const std::vector<std::int32_t>& displacements) const
  {
    using adapter = container_adapter<type>;
    all_to_all_varying(MPI_IN_PLACE, std::vector<std::int32_t>(), std::vector<std::int32_t>(), data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), sizes, displacements, adapter::data_type());
  }
  template <typename type>                            
  void                                      all_to_all_varying            (      type&          data    , const std::vector<std::int32_t>& sizes) const
  {
    using adapter = container_adapter<type>;
    
    std::vector<std::int32_t> displacements(sizes.size());
    std::exclusive_scan(sizes.begin(), sizes.end(), displacements.begin(), 0);

    all_to_all_varying(MPI_IN_PLACE, std::vector<std::int32_t>(), std::vector<std::int32_t>(), data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), sizes, displacements, adapter::data_type());
  }
  [[nodiscard]]
  request                                   immediate_all_to_all_varying  (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type& sent_data_type    ,
                                                                                 void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ialltoallv, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                          received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_, &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   immediate_all_to_all_varying  (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                                                 received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return immediate_all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
  template <typename type> [[nodiscard]]                           
  request                                   immediate_all_to_all_varying  (      type&          data    , const std::vector<std::int32_t>& sizes         , const std::vector<std::int32_t>& displacements) const
  {
    using adapter = container_adapter<type>;
    return immediate_all_to_all_varying(MPI_IN_PLACE, std::vector<std::int32_t>(), std::vector<std::int32_t>(), data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), sizes, displacements, adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_all_to_all_varying (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type& sent_data_type    ,
                                                                                 void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Alltoallv_init, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                              received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_, info.native(), &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   persistent_all_to_all_varying (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                                                 received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return persistent_all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type(), info);
  }
  template <typename type> [[nodiscard]]                           
  request                                   persistent_all_to_all_varying (      type&          data    , const std::vector<std::int32_t>& sizes         , const std::vector<std::int32_t>& displacements         , const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_all_to_all_varying(MPI_IN_PLACE, std::vector<std::int32_t>(), std::vector<std::int32_t>(), data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), sizes, displacements, adapter::data_type(), info);
  }
#endif

  // The alltoallw is the only family of functions that do not have convenience wrappers (because it just cannot be made convenient).
  void                                      all_to_all_general            (const void* sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const std::vector<MPI_Datatype>& received_data_types) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Alltoallw    ,  (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                              received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_))
  }
  [[nodiscard]]
  request                                   immediate_all_to_all_general  (const void* sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const std::vector<MPI_Datatype>& received_data_types) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ialltoallw    , (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                              received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_, &result.native_))
    return result;
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_all_to_all_general (const void* sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const std::vector<MPI_Datatype>& received_data_types, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Alltoallw_init, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                              received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_, info.native(), &result.native_))
    return result;
  }
#endif

  void                                      all_gather                    (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Allgather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_gather                    (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    all_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
  template <typename type>                            
  void                                      all_gather                    (type& data) const
  {
    using adapter = container_adapter<type>;
    all_gather(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]                                                           
  request                                   immediate_all_gather          (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Iallgather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_all_gather          (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_all_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_all_gather          (type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_all_gather(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_all_gather         (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Allgather_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   persistent_all_gather         (sent_type& sent_data, received_type& received_data, const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_all_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), info);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_all_gather         (type& data, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_all_gather(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), info);
  }
#endif

  void                                      all_gather_varying            (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Allgatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_gather_varying            (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_gather_varying            (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes                                                , const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      all_gather_varying            (const sent_type& sent, received_type& received                                                                                                 , const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    std::int32_t              local_size    (send_adapter::size(sent));
    std::vector<std::int32_t> received_sizes(size());
    all_gather(local_size, received_sizes);

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
  
  template <typename type>                            
  void                                      all_gather_varying            (      type&      data,                          const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + rank()), data.rend());
    }

    all_gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type());
  }
  template <typename type>                            
  void                                      all_gather_varying            (      type&      data,                          const std::vector<std::int32_t>& received_sizes                                                , const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + rank()), data.rend());
    }

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    all_gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type());
  }
  template <typename type>
  void                                      all_gather_varying            (      type&      data                                                                                                                          , const bool resize = false) const
  {
    using adapter = container_adapter<type>;

    std::int32_t              local_size    (adapter::size(data));
    std::vector<std::int32_t> received_sizes(size());
    all_gather(local_size, received_sizes);

    if (resize)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + rank()), data.rend());
    }

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    all_gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type());
  }
  [[nodiscard]]
  request                                   immediate_all_gather_varying  (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Iallgatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   immediate_all_gather_varying  (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return immediate_all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
  template <typename type> [[nodiscard]]                           
  request                                   immediate_all_gather_varying  (      type&      data,                          const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const bool resize = false) const
  {
    using adapter = container_adapter<type>;

    if (resize)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + rank()), data.rend());
    }

    return immediate_all_gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_all_gather_varying (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Allgatherv_init, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   persistent_all_gather_varying (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,            const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return persistent_all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type(), info);
  }
  template <typename type> [[nodiscard]]                           
  request                                   persistent_all_gather_varying (      type&      data,                          const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,            const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using adapter = container_adapter<type>;

    if (resize)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + rank()), data.rend());
    }

    return persistent_all_gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type(), info);
  }
#endif

  void                                      all_reduce                    (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Allreduce, (sent, received, size, data_type.native(), op.native(), native_))
  }
  template <typename type>                            
  void                                      all_reduce                    (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    all_reduce(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type>                            
  void                                      all_reduce                    (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    all_reduce(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
  [[nodiscard]]
  request                                   immediate_all_reduce          (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Iallreduce, (sent, received, size, data_type.native(), op.native(), native_, &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_all_reduce          (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_all_reduce(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type> [[nodiscard]]                           
  request                                   immediate_all_reduce          (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_all_reduce(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_all_reduce         (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Allreduce_init, (sent, received, size, data_type.native(), op.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_all_reduce         (const type& sent, type& received,                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_all_reduce(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op, info);
  }
  template <typename type> [[nodiscard]]                           
  request                                   persistent_all_reduce         (      type& data,                                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_all_reduce(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op, info);
  }
#endif

  void                                      reduce_scatter                (const void* sent, void* received, const std::vector<std::int32_t>& sizes, const data_type& data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Reduce_scatter, (sent, received, sizes.data(), data_type.native(), op.native(), native_))
  }
  template <typename type>                            
  void                                      reduce_scatter                (const type& sent, type& received, const std::vector<std::int32_t>& sizes,                             const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    reduce_scatter(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), sizes, adapter::data_type(), op);
  }
  template <typename type>                            
  void                                      reduce_scatter                (      type& data,                 const std::vector<std::int32_t>& sizes,                             const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    reduce_scatter(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), sizes, adapter::data_type(), op);
  }
  [[nodiscard]]
  request                                   immediate_reduce_scatter      (const void* sent, void* received, const std::vector<std::int32_t>& sizes, const data_type& data_type, const op& op = ops::sum) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ireduce_scatter, (sent, received, sizes.data(), data_type.native(), op.native(), native_, &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_reduce_scatter      (const type& sent, type& received, const std::vector<std::int32_t>& sizes,                             const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_reduce_scatter(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), sizes, adapter::data_type(), op);
  }
  template <typename type> [[nodiscard]]                           
  request                                   immediate_reduce_scatter      (      type& data,                 const std::vector<std::int32_t>& sizes,                             const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_reduce_scatter(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), sizes, adapter::data_type(), op);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_reduce_scatter     (const void* sent, void* received, const std::vector<std::int32_t>& sizes, const data_type& data_type, const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Reduce_scatter_init, (sent, received, sizes.data(), data_type.native(), op.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_reduce_scatter     (const type& sent, type& received, const std::vector<std::int32_t>& sizes,                             const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_reduce_scatter(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), sizes, adapter::data_type(), op, info);
  }
  template <typename type> [[nodiscard]]                           
  request                                   persistent_reduce_scatter     (      type& data,                 const std::vector<std::int32_t>& sizes,                             const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_reduce_scatter(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), sizes, adapter::data_type(), op, info);
  }
#endif

  void                                      reduce_scatter_block          (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Reduce_scatter_block, (sent, received, size, data_type.native(), op.native(), native_))
  }
  template <typename type>                            
  void                                      reduce_scatter_block          (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    reduce_scatter_block(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type>                            
  void                                      reduce_scatter_block          (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    reduce_scatter_block(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
  [[nodiscard]]
  request                                   immediate_reduce_scatter_block(const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ireduce_scatter_block, (sent, received, size, data_type.native(), op.native(), native_, &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_reduce_scatter_block(const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_reduce_scatter_block(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type> [[nodiscard]]                           
  request                                   immediate_reduce_scatter_block(      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_reduce_scatter_block(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_reduce_scatter_block(const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Reduce_scatter_block_init, (sent, received, size, data_type.native(), op.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_reduce_scatter_block(const type& sent, type& received,                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_reduce_scatter_block(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op, info);
  }
  template <typename type> [[nodiscard]]                           
  request                                   persistent_reduce_scatter_block(      type& data,                                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_reduce_scatter_block(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op, info);
  }
#endif

  // All to one collective operations.

  void                                      gather                        (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Gather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), root, native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      gather                        (sent_type& sent_data, received_type& received_data, const std::int32_t root = 0) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), root);
  }
  template <typename type>                            
  void                                      gather                        (type& data, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    gather(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), root);
  }
  [[nodiscard]]                                                           
  request                                   immediate_gather              (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type,  
                                                                           const std::int32_t root = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Igather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), root, native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_gather              (sent_type& sent_data, received_type& received_data, const std::int32_t root = 0) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), root);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_gather              (type& data, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_gather(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), root);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_gather             (const void*        sent        , const std::int32_t      sent_size     , const data_type& sent_data_type    ,
                                                                                 void*        received    , const std::int32_t      received_size , const data_type& received_data_type,  
                                                                           const std::int32_t root     = 0, const mpi::information& info          = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Gather_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), root, native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   persistent_gather             (sent_type& sent_data, received_type& received_data, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), root, info);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_gather             (type& data,                                         const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_gather(
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL),
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), root, info);
  }
#endif

  void                                      gather_varying                (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type, const std::int32_t root = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Gatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), root, native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      gather_varying                (sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const std::int32_t root = 0, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize && rank() == root)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type(), root);
  }
  template <typename sent_type, typename received_type>                            
  void                                      gather_varying                (sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes,                                                 const std::int32_t root = 0, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize && rank() == root)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type(), root);
  }
  template <typename sent_type, typename received_type>                            
  void                                      gather_varying                (sent_type& sent, received_type& received,                                                                                                  const std::int32_t root = 0, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    std::int32_t              local_size    (send_adapter::size(sent));
    std::vector<std::int32_t> received_sizes(size());
    gather(local_size, received_sizes, root);

    if (resize && rank() == root)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type(), root);
  }
  template <typename type>                            
  void                                      gather_varying                (type&      data,                          const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const std::int32_t root = 0, const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize && rank() == root)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + root), data.rend());
    }

    gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type(), root);
  }
  template <typename type>                            
  void                                      gather_varying                (type&      data,                          const std::vector<std::int32_t>& received_sizes,                                                 const std::int32_t root = 0, const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize && rank() == root)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + root), data.rend());
    }

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type(), root);
  }
  template <typename type>                            
  void                                      gather_varying                (type&      data,                                                                                                                           const std::int32_t root = 0, const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    std::int32_t              local_size    (adapter::size(data));
    std::vector<std::int32_t> received_sizes(size());
    gather(local_size, received_sizes, root);

    if (resize && rank() == root)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + root), data.rend());
    }

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type(), root);
  }
  [[nodiscard]]
  request                                   immediate_gather_varying      (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type, const std::int32_t root = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Igatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), root, native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   immediate_gather_varying      (sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const std::int32_t root = 0, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize && rank() == root)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return immediate_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type(), root);
  }
  template <typename type> [[nodiscard]]                           
  request                                   immediate_gather_varying      (type&      data,                          const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const std::int32_t root = 0, const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize && rank() == root)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + root), data.rend());
    }

    return immediate_gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type(), root);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_gather_varying     (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Gatherv_init, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), root, native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   persistent_gather_varying     (sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,                  const std::int32_t root = 0, const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize && rank() == root)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return persistent_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type(), root, info);
  }
  template <typename type> [[nodiscard]]                           
  request                                   persistent_gather_varying     (type&      data,                          const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,                  const std::int32_t root = 0, const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize && rank() == root)
    {
      adapter::resize(data, std::reduce(received_sizes.begin(), received_sizes.end()));
      std::rotate(data.rbegin(), data.rbegin() + std::reduce(received_sizes.begin(), received_sizes.begin() + root), data.rend());
    }

    return persistent_gather_varying(MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), static_cast<void*>(adapter::data(data)), received_sizes, displacements, adapter::data_type(), root, info);
  }
#endif

  void                                      reduce_local                  (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Reduce_local, (sent, received, size, data_type.native(), op.native()))
  }
  template <typename type>                            
  void                                      reduce_local                  (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    reduce_local(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type>                            
  void                                      reduce_local                  (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    reduce_local(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
  
  void                                      reduce                        (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum, const std::int32_t root = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Reduce, (sent, received, size, data_type.native(), op.native(), root, native_))
  }
  template <typename type>                            
  void                                      reduce                        (const type& sent, type& received,                                                      const op& op = ops::sum, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    reduce(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op, root);
  }
  template <typename type>                            
  void                                      reduce                        (      type& data,                                                                      const op& op = ops::sum, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    reduce(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op, root);
  }
  [[nodiscard]]
  request                                   immediate_reduce              (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum, const std::int32_t root = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ireduce, (sent, received, size, data_type.native(), op.native(), root, native_, &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_reduce              (const type& sent, type& received,                                                      const op& op = ops::sum, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_reduce(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op, root);
  }
  template <typename type> [[nodiscard]]                           
  request                                   immediate_reduce              (      type& data,                                                                      const op& op = ops::sum, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_reduce(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op, root);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_reduce             (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Reduce_init, (sent, received, size, data_type.native(), op.native(), root, native_, info.native(), &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_reduce             (const type& sent, type& received,                                                      const op& op = ops::sum, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_reduce(static_cast<void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op, root, info);
  }
  template <typename type> [[nodiscard]]                           
  request                                   persistent_reduce             (      type& data,                                                                      const op& op = ops::sum, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_reduce(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op, root, info);
  }
#endif
  
  // One to all collective operations.
                                
  void                                      broadcast                     (void* data, const std::int32_t count, const data_type& data_type, const std::int32_t root = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Bcast, (data, count, data_type.native(), root, native_))
  }
  template <typename type>                                                
  void                                      broadcast                     (type& data, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    broadcast(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), root);
  }
  [[nodiscard]]                                                           
  request                                   immediate_broadcast           (void* data, const std::int32_t count, const data_type& data_type, const std::int32_t root = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ibcast, (data, count, data_type.native(), root, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_broadcast           (type& data, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_broadcast(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), root);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_broadcast          (void* data, const std::int32_t count, const data_type& data_type, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Bcast_init, (data, count, data_type.native(), root, native_, info.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_broadcast          (type& data,                                                       const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_broadcast(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), root, info);
  }
#endif

  void                                      scatter                       (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Scatter, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), root, native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      scatter                       (sent_type& sent_data, received_type& received_data, const std::int32_t root = 0) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    scatter(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), root);
  }
  template <typename type>                            
  void                                      scatter                       (type& data, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    scatter(
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), 
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), root);
  }
  [[nodiscard]]                                                           
  request                                   immediate_scatter             (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Iscatter, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), root, native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_scatter             (sent_type& sent_data, received_type& received_data, const std::int32_t root = 0) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_scatter(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), root);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_scatter             (type& data, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_scatter(
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), 
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), root);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_scatter            (const void*        sent       , const std::int32_t      sent_size     , const data_type& sent_data_type    ,
                                                                                 void*        received   , const std::int32_t      received_size , const data_type& received_data_type, 
                                                                           const std::int32_t root    = 0, const mpi::information& info          = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Scatter_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), root, native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   persistent_scatter            (sent_type& sent_data, received_type& received_data, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_scatter(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), root, info);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_scatter            (type& data                                        , const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_scatter(
      static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), 
      MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), root, info);
  }
#endif

  void                                      scatter_varying               (const void* sent    , const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements, const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size                                                           , const data_type& received_data_type, const std::int32_t root = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Scatterv, (sent, sent_sizes.data(), displacements.data(), sent_data_type.native(), received, received_size, received_data_type.native(), root, native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      scatter_varying               (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements           , const std::int32_t root = 0, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    
    if (resize)
    {
      std::int32_t local_size(0);
      scatter(sent_sizes, local_size, root);
      receive_adapter::resize(received, local_size);
    }

    scatter_varying(
      static_cast<void*>(send_adapter   ::data(sent    )), sent_sizes, displacements                                 , send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), root);
  }
  template <typename sent_type, typename received_type>                                                                                                                                                                          
  void                                      scatter_varying               (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& sent_sizes                                                           , const std::int32_t root = 0, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
    {
      std::int32_t local_size(0);
      scatter(sent_sizes, local_size, root);
      receive_adapter::resize(received, local_size);
    }

    std::vector<std::int32_t> displacements(sent_sizes.size());
    std::exclusive_scan(sent_sizes.begin(), sent_sizes.end(), displacements.begin(), 0);

    scatter_varying(
      static_cast<void*>(send_adapter   ::data(sent    )), sent_sizes, displacements                                 , send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), root);
  }
  template <typename type>                                                                                                                                                                                                       
  void                                      scatter_varying               (      type&      data,                          const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements           , const std::int32_t root = 0, const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize)
    {
      std::int32_t local_size(0);
      scatter(sent_sizes, local_size, root);
      if (rank() != root)
        adapter::resize(data, local_size);
    }

    scatter_varying(static_cast<void*>(adapter::data(data)), sent_sizes, displacements, adapter::data_type(), MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), root);
  }
  template <typename type>                                                                                                                                                                                                       
  void                                      scatter_varying               (      type&      data,                          const std::vector<std::int32_t>& sent_sizes                                                           , const std::int32_t root = 0, const bool resize = false) const
  {
    using adapter = container_adapter<type>;
    
    if (resize)
    {
      std::int32_t local_size(0);
      scatter(sent_sizes, local_size, root);
      if (rank() != root)
        adapter::resize(data, local_size);
    }

    std::vector<std::int32_t> displacements(sent_sizes.size());
    std::exclusive_scan(sent_sizes.begin(), sent_sizes.end(), displacements.begin(), 0);

    scatter_varying(static_cast<void*>(adapter::data(data)), sent_sizes, displacements, adapter::data_type(), MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), root);
  }
  [[nodiscard]]                                                           
  request                                   immediate_scatter_varying     (const void* sent    , const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements, const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size                                                           , const data_type& received_data_type, const std::int32_t root = 0) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Iscatterv, (sent, sent_sizes.data(), displacements.data(), sent_data_type.native(), received, received_size, received_data_type.native(), root, native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_scatter_varying     (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements           , const std::int32_t root = 0) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_scatter_varying(
      static_cast<void*>(send_adapter   ::data(sent    )), sent_sizes, displacements                                 , send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), root);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_scatter_varying     (      type&      data,                          const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements           , const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_scatter_varying(static_cast<void*>(adapter::data(data)), sent_sizes, displacements, adapter::data_type(), MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), root);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_scatter_varying    (const void* sent    , const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements, const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size                                                           , const data_type& received_data_type, const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Scatterv_init, (sent, sent_sizes.data(), displacements.data(), sent_data_type.native(), received, received_size, received_data_type.native(), root, native_, info, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   persistent_scatter_varying    (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements           , const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_scatter_varying(
      static_cast<void*>(send_adapter   ::data(sent    )), sent_sizes, displacements                                 , send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), root, info);
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   persistent_scatter_varying    (      type&      data,                          const std::vector<std::int32_t>& sent_sizes, const std::vector<std::int32_t>& displacements           , const std::int32_t root = 0, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_scatter_varying(static_cast<void*>(adapter::data(data)), sent_sizes, displacements, adapter::data_type(), MPI_IN_PLACE, 0, data_type(MPI_DATATYPE_NULL), root, info);
  }
#endif

  // Other collective operations.

  void                                      inclusive_scan                (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Scan, (sent, received, size, data_type.native(), op.native(), native_))
  }
  template <typename type>                            
  void                                      inclusive_scan                (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    inclusive_scan(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type>                            
  void                                      inclusive_scan                (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    inclusive_scan(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
  [[nodiscard]]
  request                                   immediate_inclusive_scan      (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Iscan, (sent, received, size, data_type.native(), op.native(), native_, &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_inclusive_scan      (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_inclusive_scan(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_inclusive_scan      (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_inclusive_scan(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_inclusive_scan     (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Scan_init, (sent, received, size, data_type.native(), op.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_inclusive_scan     (const type& sent, type& received,                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_inclusive_scan(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op, info);
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_inclusive_scan     (      type& data,                                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_inclusive_scan(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op, info);
  }
#endif
  
  void                                      exclusive_scan                (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Exscan, (sent, received, size, data_type.native(), op.native(), native_))
  }
  template <typename type>                            
  void                                      exclusive_scan                (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    exclusive_scan(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type>                            
  void                                      exclusive_scan                (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    exclusive_scan(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
  [[nodiscard]]
  request                                   immediate_exclusive_scan      (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Iexscan, (sent, received, size, data_type.native(), op.native(), native_, &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_exclusive_scan      (const type& sent, type& received,                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_exclusive_scan(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op);
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_exclusive_scan      (      type& data,                                                                      const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return immediate_exclusive_scan(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_exclusive_scan     (const void* sent, void* received, const std::int32_t size, const data_type& data_type, const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Exscan_init, (sent, received, size, data_type.native(), op.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_exclusive_scan     (const type& sent, type& received,                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_exclusive_scan(static_cast<const void*>(adapter::data(sent)), static_cast<void*>(adapter::data(received)), static_cast<std::int32_t>(adapter::size(sent)), adapter::data_type(), op, info);
  }
  template <typename type> [[nodiscard]]
  request                                   persistent_exclusive_scan     (      type& data,                                                                      const op& op = ops::sum, const mpi::information& info = mpi::information()) const
  {
    using adapter = container_adapter<type>;
    return persistent_exclusive_scan(MPI_IN_PLACE, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), op, info);
  }
#endif
  
  // All to all neighborhood collective operations.

  void                                      neighbor_all_to_all           (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoall, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_to_all           (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    neighbor_all_to_all(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
  [[nodiscard]]                                                           
  request                                   immediate_neighbor_all_to_all (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_alltoall, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_neighbor_all_to_all (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_neighbor_all_to_all(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_neighbor_all_to_all(const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoall_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   persistent_neighbor_all_to_all(sent_type& sent_data, received_type& received_data, const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_neighbor_all_to_all(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), info);
  }
#endif

  void                                      neighbor_all_to_all_varying   (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type& sent_data_type    ,
                                                                                 void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallv, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                                  received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_to_all_varying   (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                                                 received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    neighbor_all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_to_all_varying   (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , 
                                                                                 received_type& received, const std::vector<std::int32_t>& received_sizes, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> sent_displacements    (sent_sizes    .size());
    std::vector<std::int32_t> received_displacements(received_sizes.size());
    std::exclusive_scan(sent_sizes    .begin(), sent_sizes    .end(), sent_displacements    .begin(), 0);
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), received_displacements.begin(), 0);

    neighbor_all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_to_all_varying   (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , 
                                                                                 received_type& received                                                 , const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    std::vector<std::int32_t> received_sizes(sent_sizes.size());
    neighbor_all_to_all(sent_sizes, received_sizes);

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> sent_displacements    (sent_sizes    .size());
    std::vector<std::int32_t> received_displacements(received_sizes.size());
    std::exclusive_scan(sent_sizes    .begin(), sent_sizes    .end(), sent_displacements    .begin(), 0);
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), received_displacements.begin(), 0);

    neighbor_all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
  [[nodiscard]]
  request                                   immediate_neighbor_all_to_all_varying (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type& sent_data_type    ,
                                                                                         void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_alltoallv, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                                   received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_, &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   immediate_neighbor_all_to_all_varying (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                                                         received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return immediate_neighbor_all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_neighbor_all_to_all_varying(const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type& sent_data_type    ,
                                                                                         void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallv_init, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                                       received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_, info.native(), &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   persistent_neighbor_all_to_all_varying(const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                                                         received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return persistent_neighbor_all_to_all_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, received_displacements, receive_adapter::data_type(), info);
  }
#endif

  // The alltoallw is the only family of functions that do not have convenience wrappers (because it just cannot be made convenient).
  void                                      neighbor_all_to_all_general           (const void* sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<aint>& sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                                                         void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<aint>& received_displacements, const std::vector<MPI_Datatype>& received_data_types) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallw     ,  (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                                        received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_))
  }
  [[nodiscard]]
  request                                   immediate_neighbor_all_to_all_general (const void* sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<aint>& sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                                                         void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<aint>& received_displacements, const std::vector<MPI_Datatype>& received_data_types) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_alltoallw    , (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                                       received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_, &result.native_))
    return result;
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_neighbor_all_to_all_general(const void* sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<aint>& sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                                                         void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<aint>& received_displacements, const std::vector<MPI_Datatype>& received_data_types, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallw_init, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                                       received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_, info.native(), &result.native_))
    return result;
  }
#endif

  void                                      neighbor_all_gather           (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_gather           (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    neighbor_all_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
  [[nodiscard]]                                                           
  request                                   immediate_neighbor_all_gather (const void* sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void* received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_allgather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_neighbor_all_gather (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_neighbor_all_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request                                   persistent_neighbor_all_gather(const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgather_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   persistent_neighbor_all_gather(sent_type& sent_data, received_type& received_data, const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_neighbor_all_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), info);
  }
#endif

  void                                      neighbor_all_gather_varying   (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                 void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_gather_varying   (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    neighbor_all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_gather_varying   (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes                                                , const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    neighbor_all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void                                      neighbor_all_gather_varying   (const sent_type& sent, received_type& received                                                                                                 , const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    std::int32_t              local_size    (send_adapter::size(sent));
    std::vector<std::int32_t> received_sizes(size()); // Over allocation (whole communicator instead of neighbors as the number of neighbors is inaccessible).
    neighbor_all_gather(local_size, received_sizes);

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    neighbor_all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
  [[nodiscard]]
  request                                   immediate_neighbor_all_gather_varying  (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                          void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_allgatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   immediate_neighbor_all_gather_varying  (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return immediate_neighbor_all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   persistent_neighbor_all_gather_varying (const void* sent    , const std::int32_t               sent_size     ,                                                 const data_type& sent_data_type    ,
                                                                                          void* received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, const data_type& received_data_type, const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgatherv_init, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request                                   persistent_neighbor_all_gather_varying (const sent_type& sent, received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,            const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return persistent_neighbor_all_gather_varying(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      static_cast<      void*>(receive_adapter::data(received)), received_sizes, displacements                      , receive_adapter::data_type(), info);
  }
#endif

  [[nodiscard]]                                                           
  bool                                      managed                       () const
  {
    return managed_;
  }
  [[nodiscard]]                                                           
  MPI_Comm                                  native                        () const
  {
    return native_;
  }

protected:
  bool     managed_ = false;
  MPI_Comm native_  = MPI_COMM_NULL;
};

inline const communicator world_communicator(MPI_COMM_WORLD);
inline const communicator self_communicator (MPI_COMM_SELF );

// Convenience for world communicator predefined attributes.
inline std::int32_t application_number ()
{
  return *world_communicator.attribute<std::int32_t>(communicator_key_value(MPI_APPNUM));
}
inline std::int32_t universe_size      ()
{
  return *world_communicator.attribute<std::int32_t>(communicator_key_value(MPI_UNIVERSE_SIZE));
}
inline error_code   last_used_code     ()
{
  return *world_communicator.attribute<error_code>  (communicator_key_value(MPI_LASTUSEDCODE));
}
inline std::int32_t tag_upper_bound    ()
{
  return *world_communicator.attribute<std::int32_t>(communicator_key_value(MPI_TAG_UB));
}
inline std::int32_t host_rank          ()
{
  return *world_communicator.attribute<std::int32_t>(communicator_key_value(MPI_HOST));
}
inline std::int32_t io_rank            ()
{
  return *world_communicator.attribute<std::int32_t>(communicator_key_value(MPI_IO));
}
inline bool         wall_time_is_global()
{
  return *world_communicator.attribute<bool>(communicator_key_value(MPI_WTIME_IS_GLOBAL));
}
}