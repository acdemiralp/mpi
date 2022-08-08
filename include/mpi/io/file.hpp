#pragma once

#include <cstdint>
#include <type_traits>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/type/data_type.hpp>
#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/request.hpp>
#include <mpi/core/status.hpp>
#include <mpi/io/enums/access_mode.hpp>
#include <mpi/io/enums/seek_mode.hpp>
#include <mpi/io/structs/file_view.hpp>
#include <mpi/io/data_representation.hpp>

namespace mpi::io
{
class file
{
public:
  explicit file  (const communicator& communicator, const std::string& filepath, const access_mode access_mode, const information& information = mpi::information())
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_File_open, (communicator.native(), filepath.c_str(), static_cast<std::int32_t>(access_mode), information.native(), &native_))
  }
  explicit file  (MPI_File native, const bool managed = false)
  : managed_(managed), native_(native)
  {

  }
  file           (const file&  that) = delete;
  file           (      file&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_FILE_NULL;
  }
  virtual ~file  () noexcept(false)
  {
    if (managed_ && native_ != MPI_FILE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_File_close, (&native_))
  }
  file& operator=(const file&  that) = delete;
  file& operator=(      file&& temp) noexcept(false)
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_FILE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_File_close, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_ ;

      temp.managed_ = false;
      temp.native_  = MPI_FILE_NULL;
    }
    return *this;
  }

  [[nodiscard]]
  access_mode                          access_mode           () const
  {
    io::access_mode result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_amode, (native_, reinterpret_cast<std::int32_t*>(&result)))
    return result;
  }
  [[nodiscard]]                                              
  offset                               byte_offset           (const offset value) const
  {
    offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_byte_offset    , (native_, value, &result))
    return result;
  }
  [[nodiscard]]                                              
  aint                                 extent                (const data_type& type) const
  {
    aint result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_type_extent, (native_, type.native(), &result))
    return result;
  }
  [[nodiscard]]                                              
  group                                group                 () const
  {
    mpi::group result(MPI_GROUP_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_get_group, (native_, &result.native_))
    return result;
  }
  [[nodiscard]]                                              
  offset                               position              () const
  {
    offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_position       , (native_, &result))
    return result;
  }
  [[nodiscard]]                                              
  offset                               shared_position       () const
  {
    offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_position_shared, (native_, &result))
    return result;
  }
                                                             
  [[nodiscard]]                                              
  information                          information           () const
  {
    mpi::information result(MPI_INFO_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_get_info, (native_, &result.native_))
    return result;
  }
  void                                 set_information       (const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_info, (native_, value.native()))
  }
                                                             
  [[nodiscard]]                                              
  offset                               size                  () const
  {
    offset result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_size, (native_, &result))
    return result;
  }
  void                                 set_size              (const offset value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_size, (native_, value))
  }
                                                             
  [[nodiscard]]                                              
  bool                                 atomic                () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_File_get_atomicity, (native_, &result))
    return static_cast<bool>(result);
  }
  void                                 set_atomic            (const bool value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_atomicity, (native_, value))
  }
                                                             
  [[nodiscard]]                                              
  file_view                            view                  () const
  {
    file_view   result;
    std::string representation_name(MPI_MAX_DATAREP_STRING, '\n');
    MPI_CHECK_ERROR_CODE(MPI_File_get_view, (native_, &result.displacement, &result.elementary_type.native_, &result.file_type.native_, representation_name.data()))
    result.representation = data_representation(representation_name);
    return result;
  }
  void                                 set_view              (const file_view& view, const mpi::information& information = mpi::information()) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_view, (native_, view.displacement, view.elementary_type.native(), view.file_type.native(), view.representation.name().c_str(), information.native()))
  }
                                                             
  [[nodiscard]]                                              
  file_error_handler                   error_handler         () const
  {
    file_error_handler result(MPI_ERRHANDLER_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_get_errhandler, (native_, &result.native_))
    return result;
  }
  void                                 set_error_handler     (const file_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_set_errhandler, (native_, value.native()))
  }
  void                                 call_error_handler    (const error_code& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_call_errhandler, (native_, value.native()))
  }
                                                             
  void                                 preallocate           (const offset size) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_preallocate, (native_, size))
  }
  void                                 synchronize           () const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_sync, (native_))
  }
                                                             
  void                                 seek                  (const offset offset, const seek_mode mode = seek_mode::set) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_seek       , (native_, offset, static_cast<std::int32_t>(mode)))
  }
  void                                 seek_shared           (const offset offset, const seek_mode mode = seek_mode::set) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_seek_shared, (native_, offset, static_cast<std::int32_t>(mode)))
  }

  // Read operations.
  status                               read                  (void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>               
  status                               read                  (type& data) const
  {
    using adapter = container_adapter<type>;
    return read(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  template <typename type>
  std::pair<type, status>              read_count            (const std::int32_t count = 1) const // Named differently to avoid conflict with the type& override where type is std::int32_t.
  {
    std::pair<type, status> result;
    container_adapter<type>::resize(result.first, count);
    result.second = read(result.first);
    return result;
  }
  status                               read_all              (void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_all, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>             
  status                               read_all              (type& data) const
  {
    using adapter = container_adapter<type>;
    return read_all(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  template <typename type>
  std::pair<type, status>              read_all_count        (const std::int32_t count = 1) const
  {
    std::pair<type, status> result;
    container_adapter<type>::resize(result.first, count);
    result.second = read_all(result.first);
    return result;
  }

  void                                 read_all_begin        (void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_read_all_begin, (native_, data, count, data_type.native()))
  }
  template <typename type>                    
  void                                 read_all_begin        (type& data) const
  {
    using adapter = container_adapter<type>;
    read_all_begin(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  status                               read_all_end          (void* data) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_all_end, (native_, data, &result))
    return result;
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, void*>>> [[nodiscard]]                   
  status                               read_all_end          (type& data) const
  {
    using adapter = container_adapter<type>;
    return read_all_end(static_cast<void*>(adapter::data(data)));
  }
  status                               read_at               (const offset offset, void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_at, (native_, offset, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>                  
  status                               read_at               (const offset offset, type& data) const
  {
    using adapter = container_adapter<type>;
    return read_at(offset, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  template <typename type>
  std::pair<type, status>              read_at_count         (const offset offset, const std::int32_t count = 1) const
  {
    std::pair<type, status> result;
    container_adapter<type>::resize(result.first, count);
    result.second = read_at(offset, result.first);
    return result;
  }
  status                               read_at_all           (const offset offset, void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_at_all, (native_, offset, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>                 
  status                               read_at_all           (const offset offset, type& data) const
  {
    using adapter = container_adapter<type>;
    return read_at_all(offset, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  template <typename type>
  std::pair<type, status>              read_at_all_count     (const offset offset, const std::int32_t count = 1) const
  {
    std::pair<type, status> result;
    container_adapter<type>::resize(result.first, count);
    result.second = read_at_all(offset, result.first);
    return result;
  }

  void                                 read_at_all_begin     (const offset offset, void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_read_at_all_begin, (native_, offset, data, count, data_type.native()))
  }
  template <typename type>                    
  void                                 read_at_all_begin     (const offset offset, type& data) const
  {
    using adapter = container_adapter<type>;
    read_at_all_begin(offset, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  status                               read_at_all_end       (void* data) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_at_all_end, (native_, data, &result))
    return result;
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, void*>>>                  
  status                               read_at_all_end       (type& data) const
  {
    using adapter = container_adapter<type>;
    return read_at_all_end(static_cast<void*>(adapter::data(data)));
  }
  status                               read_ordered          (void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_ordered, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>            
  status                               read_ordered          (type& data) const
  {
    using adapter = container_adapter<type>;
    return read_ordered(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  template <typename type>
  std::pair<type, status>              read_ordered_count    (const std::int32_t count = 1) const
  {
    std::pair<type, status> result;
    container_adapter<type>::resize(result.first, count);
    result.second = read_ordered(result.first);
    return result;
  }

  void                                 read_ordered_begin    (void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_read_ordered_begin, (native_, data, count, data_type.native()))
  }
  template <typename type>                    
  void                                 read_ordered_begin    (type& data) const
  {
    using adapter = container_adapter<type>;
    read_ordered_begin(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  status                               read_ordered_end      (void* data) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_ordered_end, (native_, data, &result))
    return result;
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, void*>>> [[nodiscard]]                   
  status                               read_ordered_end      (type& data) const
  {
    using adapter = container_adapter<type>;
    return read_ordered_end(static_cast<void*>(adapter::data(data)));
  }
  status                               read_shared           (void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_read_shared, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>                  
  status                               read_shared           (type& data) const
  {
    using adapter = container_adapter<type>;
    return read_shared(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  template <typename type>
  std::pair<type, status>              read_shared_count     (const std::int32_t count = 1) const // Named differently to avoid conflict with the type& override where type is std::int32_t.
  {
    std::pair<type, status> result;
    container_adapter<type>::resize(result.first, count);
    result.second = read_shared(result.first);
    return result;
  }
  
  // Immediate read operations.
  [[nodiscard]]
  request                              immediate_read        (void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iread, (native_, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_read        (type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_read(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]
  request                              immediate_read_all    (void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iread_all, (native_, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_read_all    (type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_read_all(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]
  request                              immediate_read_at     (const offset offset, void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iread_at, (native_, offset, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_read_at     (const offset offset, type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_read_at(offset, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]
  request                              immediate_read_at_all (const offset offset, void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iread_at_all, (native_, offset, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_read_at_all (const offset offset, type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_read_at_all(offset, static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]
  request                              immediate_read_shared (void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iread_shared, (native_, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_read_shared (type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_read_shared(static_cast<void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }

  // Write operations.
  status                               write                 (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>              
  status                               write                 (const type& data) const
  {
    using adapter = container_adapter<type>;
    return write(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }                                           
  status                               write_all             (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_all, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>             
  status                               write_all             (const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_all(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
                                                             
  void                                 write_all_begin       (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_write_all_begin, (native_, data, count, data_type.native()))
  }
  template <typename type>                                   
  void                                 write_all_begin       (const type& data) const
  {
    using adapter = container_adapter<type>;
    write_all_begin(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }                                       
  status                               write_all_end         (const void* data) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_all_end, (native_, data, &result))
    return result;
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, const void*>>>
  status                               write_all_end         (const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_all_end(static_cast<const void*>(adapter::data(data)));
  }                                          
  status                               write_at              (const offset offset, const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_at, (native_, offset, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>
  status                               write_at              (const offset offset, const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_at(offset, static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  status                               write_at_all          (const offset offset, const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_at_all, (native_, offset, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>
  status                               write_at_all          (const offset offset, const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_at_all(offset, static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
                                                             
  void                                 write_at_all_begin    (const offset offset, const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_write_at_all_begin, (native_, offset, data, count, data_type.native()))
  }
  template <typename type>                                   
  void                                 write_at_all_begin    (const offset offset, const type& data) const
  {
    using adapter = container_adapter<type>;
    write_at_all_begin(offset, static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }                                             
  status                               write_at_all_end      (const void* data) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_at_all_end, (native_, data, &result))
    return result;
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, const void*>>>
  status                               write_at_all_end      (const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_at_all_end(static_cast<const void*>(adapter::data(data)));
  }                                    
  status                               write_ordered         (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_ordered, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>               
  status                               write_ordered         (const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_ordered(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
                                                             
  void                                 write_ordered_begin   (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_CHECK_ERROR_CODE(MPI_File_write_ordered_begin, (native_, data, count, data_type.native()))
  }
  template <typename type>                                   
  void                                 write_ordered_begin   (const type& data) const
  {
    using adapter = container_adapter<type>;
    write_ordered_begin(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }                                            
  status                               write_ordered_end     (const void* data) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_ordered_end, (native_, data, &result))
    return result;
  }
  template <typename type, typename = std::enable_if_t<!std::is_same_v<type, const void*>>>
  status                               write_ordered_end     (const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_ordered_end(static_cast<const void*>(adapter::data(data)));
  }                                       
  status                               write_shared          (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    MPI_Status result;
    MPI_CHECK_ERROR_CODE(MPI_File_write_shared, (native_, data, count, data_type.native(), &result))
    return result;
  }
  template <typename type>
  status                               write_shared          (const type& data) const
  {
    using adapter = container_adapter<type>;
    return write_shared(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }

  // Immediate write operations.
  [[nodiscard]]
  request                              immediate_write       (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iwrite, (native_, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_write       (const type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_write(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]                                              
  request                              immediate_write_all   (const void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iwrite_all, (native_, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_write_all   (const type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_write_all(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]                                              
  request                              immediate_write_at    (const offset offset, const void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iwrite_at, (native_, offset, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]                     
  request                              immediate_write_at    (const offset offset, const type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_write_at(offset, static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]
  request                              immediate_write_at_all(const offset offset, const void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iwrite_at_all, (native_, offset, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request                              immediate_write_at_all(const offset offset, const type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_write_at_all(offset, static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }
  [[nodiscard]]
  request                              immediate_write_shared(const void* data, const std::int32_t count, const data_type& data_type) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_File_iwrite_shared, (native_, data, count, data_type.native(), &result.native_))
    return result;
  }
  template <typename type> [[nodiscard]]
  request                              immediate_write_shared(const type& data) const
  {
    using adapter = container_adapter<type>;
    return immediate_write_shared(static_cast<const void*>(adapter::data(data)), static_cast<std::int32_t>(adapter::size(data)), adapter::data_type());
  }

  [[nodiscard]]
  bool                                 managed               () const
  {
    return managed_;
  }
  [[nodiscard]]                                              
  MPI_File                             native                () const
  {
    return native_;
  }

protected:
  bool     managed_ = false;
  MPI_File native_  = MPI_FILE_NULL;
};

inline void delete_file(const std::string& filepath, const information& information = mpi::information())
{
  MPI_CHECK_ERROR_CODE(MPI_File_delete, (filepath.c_str(), information.native()))
}
}