#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/enums/mode.hpp>
#include <mpi/core/enums/window_flavor.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/structs/window_information.hpp>
#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/key_value.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/op.hpp>
#include <mpi/core/standard_ops.hpp>
#include <mpi/core/request.hpp>

namespace mpi
{
class window
{
public:
  explicit window  (const communicator& communicator, const aint size, const std::int32_t displacement_unit = 1, const bool shared = false, const information& information = mpi::information())
  : managed_(true)
  {
    void* base_pointer; // Unused. Call base_pointer() explicitly.
    if (shared)
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate_shared, (size, displacement_unit, information.native(), communicator.native(), &base_pointer, &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate       , (size, displacement_unit, information.native(), communicator.native(), &base_pointer, &native_))
  }

  explicit window  (const communicator& communicator, void* base_pointer, const aint size, const std::int32_t displacement_unit = 1, const information& information = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create, (base_pointer, size, displacement_unit, information.native(), communicator.native(), &native_))
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, void>>>
  explicit window  (const communicator& communicator, type* base_pointer, const aint size,                                           const information& information = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create, (static_cast<void*>(base_pointer), sizeof(type) * size, sizeof(type), information.native(), communicator.native(), &native_))
  }

  explicit window  (const communicator& communicator, const information& information = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_create_dynamic, (information.native(), communicator.native(), &native_))
  }

  explicit window  (const MPI_Win  native, const bool managed = false)
  : managed_(managed), native_(native)
  {
    
  }
  window           (const window&  that) = delete;
  window           (      window&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_      = false;
    temp.native_       = MPI_WIN_NULL;
  }
  virtual ~window  () noexcept(false)
  {
    if (managed_ && native_ != MPI_WIN_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Win_free, (&native_))
  }
  window& operator=(const window&  that) = delete;
  window& operator=(      window&& temp) noexcept(false)
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_WIN_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Win_free, (&native_))

      managed_      = temp.managed_     ;
      native_       = temp.native_      ;

      temp.managed_ = false;
      temp.native_  = MPI_WIN_NULL;
    }
    return *this;
  }

  // A static member function for construction is bad practice but constructors do not support templates if the type does not appear in the arguments.
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, void>>>
  static window allocate (const communicator& communicator, const aint size = 1, const bool shared = false, const information& information = mpi::information())
  {
    window result;
    void*  base_pointer; // Unused. Call base_pointer() explicitly.
    if (shared)
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate_shared, (sizeof(type) * size, sizeof(type), information.native(), communicator.native(), &base_pointer, &result.native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Win_allocate       , (sizeof(type) * size, sizeof(type), information.native(), communicator.native(), &base_pointer, &result.native_))
    return result;
  }

  [[nodiscard]]
  group                group                 () const
  {
    mpi::group result(MPI_GROUP_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Win_get_group, (native_, &result.native_))
    return result;
  }
  [[nodiscard]]                              
  window_information   query_shared          (const std::int32_t rank) const
  {
    window_information result {};
    MPI_CHECK_ERROR_CODE(MPI_Win_shared_query, (native_, rank, &result.size, &result.displacement, &result.base))
    return result;
  }
                                             
  [[nodiscard]]                              
  std::string          name                  () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, '\n');
    std::int32_t length(0);
    MPI_CHECK_ERROR_CODE(MPI_Win_get_name, (native_, &result[0], &length))
    result.resize(static_cast<std::size_t>(length));
    return result;
  }
  void                 set_name              (const std::string&      value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_name, (native_, value.c_str()))
  }
                                             
  [[nodiscard]]                              
  information          information           () const
  {
    mpi::information result(MPI_INFO_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Win_get_info, (native_, &result.native_))
    return result;
  }
  void                 set_information       (const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_info, (native_, value.native()))
  }
                                             
  [[nodiscard]]                              
  window_error_handler error_handler         () const
  {
    window_error_handler result(MPI_ERRHANDLER_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Win_get_errhandler, (native_, &result.native_))
    return result;
  }
  void                 set_error_handler     (const window_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_errhandler, (native_, value.native()))
  }
  void                 call_error_handler    (const error_code& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_call_errhandler, (native_, value.native()))
  }
                                             
  template <typename type> [[nodiscard]]
  std::optional<type>  attribute             (const window_key_value& key) const
  {
    type*        result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Win_get_attr   , (native_, key.native(), static_cast<void*>(&result), &exists))
    return static_cast<bool>(exists) ? *result : std::optional<type>(std::nullopt);
  }
  template <typename type>                   
  void                 set_attribute         (const window_key_value& key, const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_set_attr   , (native_, key.native(), static_cast<void*>(&value)))
  }
  void                 remove_attribute      (const window_key_value& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_delete_attr, (native_, key.native()))
  }

  // Convenience for predefined attributes.
  template <typename type = void> [[nodiscard]]
  type*                base_pointer          () const
  {
    return static_cast<type*>(*attribute<void*>(window_key_value(MPI_WIN_BASE)));
  }
  [[nodiscard]]                              
  aint                 size                  () const
  {
    return *attribute<aint>(window_key_value(MPI_WIN_SIZE));
  }
  [[nodiscard]]                              
  std::int32_t         displacement_unit     () const
  {
    return *attribute<std::int32_t>(window_key_value(MPI_WIN_DISP_UNIT));
  }
  [[nodiscard]]                              
  window_flavor        flavor                () const
  {
    return static_cast<window_flavor>(*attribute<std::int32_t>(window_key_value(MPI_WIN_CREATE_FLAVOR)));
  }
  [[nodiscard]]                              
  bool                 unified               () const
  {
    return *attribute<std::int32_t>(window_key_value(MPI_WIN_MODEL)) == MPI_WIN_UNIFIED;
  }

  void                 attach                (void* value, const aint size) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_attach, (native_, value, size))
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, void>>>
  void                 attach                (type* value, const aint size) const
  {
    attach(static_cast<void*>(value), sizeof(type) * size);
  }
  void                 detach                (const void* value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_detach, (native_, value))
  }
                                             
  void                 post                  (const mpi::group& group, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_post    , (group.native(), assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 start                 (const mpi::group& group, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_start   , (group.native(), assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 complete              () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_complete, (native_))
  }
                                             
  [[nodiscard]]                              
  bool                 test                  () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Win_test, (native_, &result))
    return static_cast<bool>(result);
  }
  void                 wait                  () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_wait, (native_))
  }
                                             
  void                 fence                 (const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_fence, (assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 synchronize           () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_sync, (native_))
  }
                                             
  void                 lock                  (const std::int32_t rank, const bool shared = false, const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_lock      , (shared ? MPI_LOCK_SHARED : MPI_LOCK_EXCLUSIVE, rank, assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 lock_all              (const std::optional<mode> assert = std::nullopt) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_lock_all  , (assert ? static_cast<std::int32_t>(*assert) : 0, native_))
  }
  void                 unlock                (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_unlock    , (rank, native_))
  }
  void                 unlock_all            () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_unlock_all, (native_))
  }
                                             
  void                 flush                 (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush          , (rank, native_))
  }
  void                 flush_all             () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_all      , (      native_))
  }
  void                 flush_local           (const std::int32_t rank) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_local    , (rank, native_))
  }
  void                 flush_local_all       () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Win_flush_local_all, (      native_))
  }

  // Remote memory access operations.
  void                 get                   (      void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Get, (source, source_size, source_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), native_))
  }
  void                 get                   (      void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Get, (source, source_size, source_data_type.native(), target_rank, target_displacement, source_size, source_data_type.native(), native_))
  }
  template <typename type>                   
  void                 get                   (      type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    using adapter = container_adapter<type>;
    get               (static_cast<void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement, target_size, target_data_type);
  }
  template <typename type>                   
  void                 get                   (      type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement = 0) const
  {
    using adapter = container_adapter<type>;
    get               (static_cast<void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement);
  }
                                             
  void                 put                   (const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Put, (source, source_size, source_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), native_))
  }                                          
  void                 put                   (const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement = 0) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Put, (source, source_size, source_data_type.native(), target_rank, target_displacement, source_size, source_data_type.native(), native_))
  }
  template <typename type>                   
  void                 put                   (const type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    using adapter = container_adapter<type>;
    put               (static_cast<const void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement, target_size, target_data_type);
  }
  template <typename type>                   
  void                 put                   (const type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement = 0) const
  {
    using adapter = container_adapter<type>;
    put               (static_cast<const void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement);
  }
                                             
  void                 accumulate            (const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Accumulate, (source, source_size, source_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), op.native(), native_))
  }                              
  void                 accumulate            (const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement = 0, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Accumulate, (source, source_size, source_data_type.native(), target_rank, target_displacement, source_size, source_data_type.native(), op.native(), native_))
  }
  template <typename type>                   
  void                 accumulate            (const type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    accumulate        (static_cast<const void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement, target_size, target_data_type, op);
  }
  template <typename type>                   
  void                 accumulate            (const type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement = 0, const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    accumulate        (static_cast<const void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement, op);
  }
                                             
  void                 get_accumulate        (const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type,
                                                    void*        result                                     , const std::int32_t result_size, const data_type& result_data_type,
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Get_accumulate, (source, source_size, source_data_type.native(), result, result_size, result_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), op.native(), native_))
  }
  template <typename source_type, typename result_type>
  void                 get_accumulate        (const source_type& source     , 
                                                    result_type& result     ,
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    using source_adapter = container_adapter<source_type>;
    using result_adapter = container_adapter<result_type>;
    get_accumulate    (
      static_cast<const void*>(source_adapter::data(source)), static_cast<std::int32_t>(source_adapter::size(source)), source_adapter::data_type(), 
      static_cast<      void*>(result_adapter::data(result)), static_cast<std::int32_t>(result_adapter::size(result)), result_adapter::data_type(), 
      target_rank, target_displacement, target_size, target_data_type, op);
  }
                                          
  void                 fetch_and_op          (const void* source,                      void* result, const data_type& data_type, const std::int32_t target_rank, const aint target_displacement, const op& op = ops::sum) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Fetch_and_op, (source, result, data_type.native(), target_rank, target_displacement, op.native(), native_))
  }
  template <typename type>
  void                 fetch_and_op          (const type& source,                      type& result                            , const std::int32_t target_rank, const aint target_displacement, const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    fetch_and_op(static_cast<const void*>(adapter::data(source)), static_cast<void*>(adapter::data(result)), adapter::data_type(), target_rank, target_displacement, op);
  }
                                  
  void                 compare_and_swap      (const void* source, const void* compare, void* result, const data_type& data_type, const std::int32_t target_rank, const aint target_displacement) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Compare_and_swap, (source, compare, result, data_type.native(), target_rank, target_displacement, native_))
  }
  template <typename type>
  void                 compare_and_swap      (const type& source, const type& compare, type& result                            , const std::int32_t target_rank, const aint target_displacement) const
  {
    using adapter = container_adapter<type>;
    compare_and_swap(static_cast<const void*>(adapter::data(source)), static_cast<const void*>(adapter::data(compare)), static_cast<void*>(adapter::data(result)), adapter::data_type(), target_rank, target_displacement);
  }

  // Request remote memory access operations.
  [[nodiscard]]
  request              request_get           (      void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Rget, (source, source_size, source_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request              request_get           (      type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    using adapter = container_adapter<type>;
    return request_get(static_cast<void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement, target_size, target_data_type);
  }

  [[nodiscard]]
  request              request_put           (const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Rput, (source, source_size, source_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request              request_put           (const type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type) const
  {
    using adapter = container_adapter<type>;
    return request_put(static_cast<const void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement, target_size, target_data_type);
  }

  [[nodiscard]]        
  request              request_accumulate    (const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type, 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Raccumulate, (source, source_size, source_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), op.native(), native_, &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]] 
  request              request_accumulate    (const type&        source     , 
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    using adapter = container_adapter<type>;
    return request_accumulate(static_cast<const void*>(adapter::data(source)), static_cast<std::int32_t>(adapter::size(source)), adapter::data_type(), target_rank, target_displacement, target_size, target_data_type, op);
  }

  [[nodiscard]]
  request              request_get_accumulate(const void*        source                                     , const std::int32_t source_size, const data_type& source_data_type,
                                                    void*        result                                     , const std::int32_t result_size, const data_type& result_data_type,
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    request request(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Rget_accumulate, (source, source_size, source_data_type.native(), result, result_size, result_data_type.native(), target_rank, target_displacement, target_size, target_data_type.native(), op.native(), native_, &request.native_))
    return request;
  }
  template <typename source_type, typename result_type> [[nodiscard]]
  request              request_get_accumulate(const source_type& source     , 
                                                    result_type& result     ,
                                              const std::int32_t target_rank, const aint target_displacement, const std::int32_t target_size, const data_type& target_data_type, const op& op = ops::sum) const
  {
    using source_adapter = container_adapter<source_type>;
    using result_adapter = container_adapter<result_type>;
    return request_get_accumulate(
      static_cast<const void*>(source_adapter::data(source)), static_cast<std::int32_t>(source_adapter::size(source)), source_adapter::data_type(), 
      static_cast<      void*>(result_adapter::data(result)), static_cast<std::int32_t>(result_adapter::size(result)), result_adapter::data_type(), 
      target_rank, target_displacement, target_size, target_data_type, op);
  }

  [[nodiscard]]
  bool                 managed               () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Win              native                () const
  {
    return native_;
  }

protected:
  window() : managed_(true) { }

  bool    managed_ = false;
  MPI_Win native_  = MPI_WIN_NULL;
};
}