#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
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
#include <mpi/core/port.hpp>
#include <mpi/core/request.hpp>

namespace mpi
{
class communicator
{
public:
  explicit communicator   (const MPI_Comm       native)
  : native_(native)
  {
    
  }
  communicator            (const communicator&  that , const group&                          group                          )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create       , (that.native_, group      .native(),      &native_))
  }
  communicator            (const communicator&  that , const group&                          group                          , const std::int32_t  tag)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_group , (that.native_, group      .native(), tag, &native_))
  }
#ifdef MPI_USE_LATEST
  communicator            (                            const group&                          group                          ,  const std::string& tag      , const information& information, const communicator_error_handler& error_handler)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_from_group, (group.native(), tag.c_str(), information.native(), error_handler.native(), &native_))
  }
#endif
  communicator            (const communicator&  that , const std::int32_t                    color                          , const std::int32_t  key)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_split        , (that.native_, color, key, &native_))
  }
  communicator            (const communicator&  that , const split_type                      split_type = split_type::shared, const std::int32_t  key   = 0, const information& information       = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_split_type   , (that.native_, static_cast<std::int32_t>(split_type), key, information.native(), &native_))
  }
  communicator            (const communicator&  that , const port&                           port                           , const bool          accept   , const information& information       = mpi::information(), const std::int32_t root = 0)
  : managed_(true)
  {
    if (accept)
      MPI_CHECK_ERROR_CODE(MPI_Comm_accept , (port.name().c_str(), information.native(), root, that.native(), &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Comm_connect, (port.name().c_str(), information.native(), root, that.native(), &native_))
  }
  communicator            (const communicator&  that , const spawn_information&              spawn_info                     , const std::int32_t  root  = 0, const bool         check_error_codes = true)
  : managed_(true)
  {
    std::vector<char*> arguments(spawn_info.arguments.size());
    std::transform(spawn_info.arguments.begin(), spawn_info.arguments.end(), arguments.begin(), [ ] (const std::string& value)
    {
      return value.c_str();
    });

    std::vector<std::int32_t> error_codes(spawn_info.process_count);

    MPI_CHECK_ERROR_CODE(MPI_Comm_spawn, (spawn_info.command.c_str(), arguments.empty() ? MPI_ARGV_NULL : arguments.data(), spawn_info.process_count, spawn_info.information.native(), root, that.native(), &native_, check_error_codes ? error_codes.data() : MPI_ERRCODES_IGNORE))

    if (check_error_codes)
      for (const auto& code : error_codes)
        if (code != MPI_SUCCESS)
          throw exception("spawn", error_code(code));
  }
  communicator            (const communicator&  that , const std::vector<spawn_information>& spawn_info                     , const std::int32_t  root  = 0, const bool         check_error_codes = true)
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
      std::transform(spawn_info[i].arguments.begin(), spawn_info[i].arguments.end(), arguments[i].begin(), [ ] (const std::string& value)
      {
        return value.c_str();
      });

      commands      [i]  = const_cast<char*>(spawn_info[i].command.c_str());
      raw_arguments [i]  = arguments.back().data();
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
  communicator            (const communicator&  local, const std::int32_t                    local_leader                   , const communicator& peer     , const std::int32_t peer_leader, const std::int32_t tag = 0)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Intercomm_create, (local.native_, local_leader, peer.native_, peer_leader, tag, &native_))
  }
#ifdef MPI_USE_LATEST
  communicator            (const group&         local, const std::int32_t                    local_leader                   , const group&        peer     , const std::int32_t peer_leader, const std::string& tag, const information& information, const communicator_error_handler& error_handler)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Intercomm_create_from_groups, (local.native_, local_leader, peer.native_, peer_leader, tag.c_str(), information.native(), error_handler.native(), &native_))
  }
#endif
  communicator            (const communicator&  that , const bool                            high                           )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Intercomm_merge, (that.native_, high, &native_))
  }
  communicator            (const communicator&  that , const information&                    information                    )
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
  virtual ~communicator   ()
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
  communicator& operator= (      communicator&& temp) noexcept
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
    MPI_Comm result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_parent, (&result))
    return communicator(result);
  }
  // A static member function for construction is bad practice but the unmanaged constructor also takes a single std::int32_t (an MPI_Comm). MPI should consider using unique structs rather than integers for handles.
  [[nodiscard]]
  static communicator                       join                          (const std::int32_t socket_file_descriptor)
  {
    communicator result;
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
    mpi::group result;
    result.managed_ = true;
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
    mpi::group result;
    result.managed_ = true;
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
    std::string  result(MPI_MAX_OBJECT_NAME, ' ');
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
    mpi::information result(MPI_INFO_NULL);
    result.managed_ = true;
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
    communicator_error_handler result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_errhandler, (native_, &result.native_))
    return result;
  }
  void                                      set_error_handler             (const communicator_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_errhandler, (native_, value.native()))
  }
  void                                      call_error_handler            (const error_code& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_call_errhandler, (native_, value.native()))
  }
                                                                          
  template <typename type>                                                
  std::optional<type>                       attribute                     (const communicator_key_value& key) const
  {
    type         result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_attr   , (native_, key.native(), static_cast<void*>(&result), &exists))
    return static_cast<bool>(exists) ? result : std::optional<type>(std::nullopt);
  }
  template <typename type>                                                
  void                                      set_attribute                 (const communicator_key_value& key, const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_attr   , (native_, key.native(), static_cast<void*>(&value)))
  }
  void                                      remove_attribute              (const communicator_key_value& key) const
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
    std::pair result { communicator(), request() };
    MPI_CHECK_ERROR_CODE(MPI_Comm_idup, (native_, &result.first.native_, &result.second.native_))
    return result;
  }
#ifdef MPI_USE_LATEST                                                                 
  [[nodiscard]]                                                       
  std::pair<communicator, request>          immediate_duplicate       (const std::information& information) const
  {
    std::pair result { communicator(), request() };
    MPI_CHECK_ERROR_CODE(MPI_Comm_idup_with_info, (native_, information.native(), &result.first.native_, &result.second.native_))
    return result;
  }
#endif

  void                                      barrier                       () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Barrier, (native_))
  }
  [[nodiscard]]                                                           
  request                                   immediate_barrier             () const
  {
    request request;
    MPI_CHECK_ERROR_CODE(MPI_Ibarrier, (native_, &request.native_))
    return request;
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
    request result;
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
    request result;
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
    request result;
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
    request result;
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
  status                                    receive                       (      void* data, const std::int32_t size, const data_type& data_type, const std::int32_t source     , const std::int32_t tag = 0) const
  {
    status result;
    MPI_CHECK_ERROR_CODE(MPI_Recv, (data, size, data_type.native(), source, tag, native_, &result))
    return result;
  }
  template <typename type> [[nodiscard]]
  status                                    receive                       (      type& data,                                                      const std::int32_t source     , const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return receive(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), source, tag);
  }
  [[nodiscard]]
  request                                   immediate_receive             (      void* data, const std::int32_t size, const data_type& data_type, const std::int32_t source     , const std::int32_t tag = 0) const
  {
    request result;
    MPI_CHECK_ERROR_CODE(MPI_Irecv, (data, size, data_type.native(), source, tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request                                   immediate_receive             (      type& data,                                                      const std::int32_t source     , const std::int32_t tag = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_receive(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), source, tag);
  }

  [[nodiscard]]
  status                                    send_receive                  (const void*         sent            , const std::int32_t sent_size    , const data_type&   sent_data_type    , const std::int32_t destination, const std::int32_t send_tag   ,
                                                                                 void*         received        , const std::int32_t received_size, const data_type&   received_data_type, const std::int32_t source     , const std::int32_t receive_tag) const
  {
    status result;
    MPI_CHECK_ERROR_CODE(MPI_Sendrecv, (sent    , sent_size    , sent_data_type    .native(), destination, send_tag   ,
                                        received, received_size, received_data_type.native(), source     , receive_tag, native_, &result))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  status                                    send_receive                  (const sent_type&     sent           , const std::int32_t destination  , const std::int32_t send_tag          ,
                                                                                 received_type& received       , const std::int32_t source       , const std::int32_t receive_tag       ) const
  {
    using send_adapter    = container_adapter<sent_type    >;
    using receive_adapter = container_adapter<received_type>;
    return send_receive(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter   ::size(sent    )), send_adapter   ::data_type(), destination, send_tag   , 
      static_cast<      void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), source     , receive_tag);
  }
  [[nodiscard]]                                                                                                                                                                         
  status                                    send_receive_replace          (      void*         data            , const std::int32_t size         , const data_type&   data_type         , 
                                                                           const std::int32_t  destination     , const std::int32_t send_tag     , const std::int32_t source            , const std::int32_t receive_tag) const
  {
    status result;
    MPI_CHECK_ERROR_CODE(MPI_Sendrecv_replace, (data, size, data_type.native(), destination, send_tag, source, receive_tag, native_, &result))
    return result;
  }
  template <typename type> [[nodiscard]]                                                                                                                                                
  status                                    send_receive_replace          (const type&         data            , const std::int32_t  destination , const std::int32_t send_tag          , const std::int32_t source     , const std::int32_t receive_tag) const
  {
    using adapter = container_adapter<type>;
    return send_receive_replace(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, send_tag, source, receive_tag);
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request                                   immediate_send_receive        (const void*         sent            , const std::int32_t sent_size    , const data_type&   sent_data_type    , const std::int32_t destination, const std::int32_t send_tag   , 
                                                                                 void*         received        , const std::int32_t received_size, const data_type&   received_data_type, const std::int32_t source     , const std::int32_t receive_tag) const
  {
    request result;
    MPI_CHECK_ERROR_CODE(MPI_Isendrecv, (sent    , sent_size    , sent_data_type    .native(), destination, send_tag   ,
                                         received, received_size, received_data_type.native(), source     , receive_tag, native_, &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_send_receive        (const sent_type&     sent           , const std::int32_t destination  , const std::int32_t send_tag          , 
                                                                                 received_type& received       , const std::int32_t source       , const std::int32_t receive_tag       ) const
  {
    using send_adapter    = container_adapter<sent_type    >;
    using receive_adapter = container_adapter<received_type>;
    return immediate_send_receive(
      static_cast<const void*>(send_adapter   ::data(sent    )), static_cast<std::int32_t>(send_adapter   ::size(sent    )), send_adapter   ::data_type(), destination, send_tag    , 
      static_cast<      void*>(receive_adapter::data(received)), static_cast<std::int32_t>(receive_adapter::size(received)), receive_adapter::data_type(), source     , receive_tag);
  }
  [[nodiscard]]                                                                                                                                                                         
  request                                   immediate_send_receive_replace(      void*         data            , const std::int32_t size         , const data_type&   data_type         , 
                                                                           const std::int32_t  destination     , const std::int32_t send_tag     , const std::int32_t source            , const std::int32_t receive_tag) const
  {
    request result;
    MPI_CHECK_ERROR_CODE(MPI_Isendrecv_replace, (data, size, data_type.native(), destination, send_tag, source, receive_tag, native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                                                                                                                                                
  request                                   immediate_send_receive_replace(const type&         data            , const std::int32_t  destination , const std::int32_t send_tag          , const std::int32_t source     , const std::int32_t receive_tag) const
  {
    using adapter = container_adapter<type>;
    return immediate_send_receive_replace(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), destination, send_tag, source, receive_tag);
  }
#endif 

  [[nodiscard]]                                                       
  status                                    probe                         (const std::int32_t source, const std::int32_t tag = 0) const
  {
    status result;
    MPI_CHECK_ERROR_CODE(MPI_Probe, (source, tag, native_, &result))
    return result;
  }
  [[nodiscard]]                                                       
  std::pair<message, status>                probe_message                 (const std::int32_t source, const std::int32_t tag = 0) const
  {
    std::pair result { message(), status() };
    MPI_CHECK_ERROR_CODE(MPI_Mprobe , (source, tag, native_, &result.first.native_, &result.second))
    return result;
  }
  [[nodiscard]]                                                           
  std::optional<status>                     immediate_probe               (const std::int32_t source, const std::int32_t tag = 0) const
  {
    status       result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Iprobe, (source, tag, native_, &exists, &result))
    return static_cast<bool>(exists) ? result : std::optional<status>(std::nullopt);
  }
  [[nodiscard]]                                                           
  std::optional<std::pair<message, status>> immediate_probe_message       (const std::int32_t source, const std::int32_t tag = 0) const
  {
    std::pair    result { message(), status() };
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Improbe, (source, tag, native_, &exists, &result.first.native_, &result.second))
    return static_cast<bool>(exists) ? result : std::optional<std::pair<message, status>>(std::nullopt);
  }

  // Collective operations.                                               
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
    request request;
    MPI_CHECK_ERROR_CODE(MPI_Ibcast, (data, count, data_type.native(), root, native_, &request.native_))
    return  request;
  }
  template <typename type> [[nodiscard]]                                                           
  request                                   immediate_broadcast           (type& data, const std::int32_t root = 0) const
  {
    using adapter = container_adapter<type>;
    return immediate_broadcast(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type(), root);
  }

  void                                      gather                        (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root    = 0) const
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
  [[nodiscard]]                                                           
  request                                   immediate_gather              (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root    = 0) const
  {
    request result;
    MPI_CHECK_ERROR_CODE(MPI_Igather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), root, native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request                                   immediate_gather              (sent_type& sent_data, received_type& received_data) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_gather(
      static_cast<void*>(send_adapter   ::data(sent_data    )), static_cast<std::int32_t>(send_adapter   ::size(sent_data    )), send_adapter   ::data_type(), 
      static_cast<void*>(receive_adapter::data(received_data)), static_cast<std::int32_t>(receive_adapter::size(received_data)), receive_adapter::data_type(), root);
  }

  void                                      all_gather                    (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root    = 0) const
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
  [[nodiscard]]                                                           
  request                                   immediate_all_gather          (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type) const
  {
    request result;
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

  void                                      scatter                       (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root    = 0) const
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
  [[nodiscard]]                                                           
  request                                   immediate_scatter             (const void*        sent    , const std::int32_t sent_size    , const data_type& sent_data_type    ,
                                                                                 void*        received, const std::int32_t received_size, const data_type& received_data_type, 
                                                                           const std::int32_t root    = 0) const
  {
    request result;
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
  communicator() : managed_(true) { } // Default constructor is only available to sub classes who control the member variables explicitly.
  
  bool     managed_ = false;
  MPI_Comm native_  = MPI_COMM_NULL;
};

inline const communicator world_communicator(MPI_COMM_WORLD);
inline const communicator self_communicator (MPI_COMM_SELF );
}