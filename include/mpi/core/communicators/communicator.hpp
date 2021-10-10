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
#include <mpi/core/enums/topology.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/structs/spawn_information.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/key_value.hpp>
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
  communicator            (const communicator&  that, const group&                          group      )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create       , (that.native_, group      .native(),      &native_))
  }
  communicator            (const communicator&  that, const group&                          group      , const std::int32_t tag)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_group , (that.native_, group      .native(), tag, &native_))
  }
  communicator            (const communicator&  that, const std::int32_t                    color      , const std::int32_t key)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_split        , (that.native_, color, key, &native_))
  }
  communicator            (const communicator&  that, const std::int32_t                    split_type , const std::int32_t key      , const information& information)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_split_type   , (that.native_, split_type, key, information.native(), &native_))
  }
  communicator            (const communicator&  that, const port&                           port       , const bool         accept   , const information& information       = mpi::information(), const std::int32_t root = 0)
  : managed_(true)
  {
    if (accept)
      MPI_CHECK_ERROR_CODE(MPI_Comm_accept , (port.name().c_str(), information.native(), root, that.native(), &native_))
    else
      MPI_CHECK_ERROR_CODE(MPI_Comm_connect, (port.name().c_str(), information.native(), root, that.native(), &native_))
  }
  communicator            (const communicator&  that, const spawn_information&              spawn_info , const std::int32_t root  = 0, const bool         check_error_codes = true)
  : managed_(true)
  {
    std::vector<char*> arguments(spawn_info.arguments.size());
    std::transform(spawn_info.arguments.begin(), spawn_info.arguments.end(), arguments.begin(), [ ] (const std::string& value)
    {
      return value.c_str();
    });

    std::vector<std::int32_t> error_codes(spawn_info.process_count);

    MPI_CHECK_ERROR_CODE(MPI_Comm_spawn, (spawn_info.command.c_str(), arguments.data(), spawn_info.process_count, spawn_info.information.native(), root, that.native(), &native_, error_codes.data()))

    if (check_error_codes)
      for (auto& code : error_codes)
        if (code != MPI_SUCCESS)
          throw exception("spawn", error_code(code));
  }
  communicator            (const communicator&  that, const std::vector<spawn_information>& spawn_info , const std::int32_t root  = 0, const bool         check_error_codes = true)
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

    MPI_CHECK_ERROR_CODE(MPI_Comm_spawn_multiple, (static_cast<std::int32_t>(spawn_info.size()), commands.data(), raw_arguments.data(), process_counts.data(), infos.data(), root, that.native(), &native_, error_codes.data()))

    if (check_error_codes)
      for (auto& code : error_codes)
        if (code != MPI_SUCCESS)
          throw exception("spawn_multiple", error_code(code));
  }
  communicator            (const communicator&  that, const information&                    information)
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
  static communicator&&      parent              ()
  {
    MPI_Comm result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_parent, (&result))
    return std::move(communicator(result));
  }
  // A static member function for construction is bad practice but the unmanaged constructor also takes a single std::int32_t (an MPI_Comm). MPI should consider using unique structs rather than integers for handles.
  static communicator&&      join                (const std::int32_t socket_file_descriptor)
  {
    communicator result;
    result.managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Comm_join, (socket_file_descriptor, &result.native_))
    return std::move(result);
  }

  [[nodiscard]]
  std::int32_t               rank                () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_rank, (native_, &result))
    return result;
  }
  [[nodiscard]]
  std::int32_t               size                () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_size, (native_, &result))
    return result;
  }

  [[nodiscard]]
  group                      group               () const
  {
    MPI_Group result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_group, (native_, &result))
    return mpi::group(result);
  }
  [[nodiscard]]
  topology                   topology            () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Topo_test, (native_, &result))
    return static_cast<mpi::topology>(result);
  }

  [[nodiscard]]
  bool                       is_intercommunicator() const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_test_inter, (native_, &result))
    return static_cast<bool>(result);
  }
  [[nodiscard]]
  mpi::group                 remote_group        () const
  {
    MPI_Group result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_remote_group, (native_, &result))
    return mpi::group(result);
  }
  [[nodiscard]]
  std::int32_t               remote_size         () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_remote_size, (native_, &result))
    return result;
  }

  [[nodiscard]]
  comparison                 compare             (const communicator& that) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_compare, (native_, that.native_, &result))
    return static_cast<comparison>(result);
  }

  [[nodiscard]]
  std::string                name                () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, ' ');
    std::int32_t length(0);
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_name, (native_, &result[0], &length))
    result.resize(static_cast<std::size_t>(length));
    return result;
  }
  void                       set_name            (const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_name, (native_, value.c_str()))
  }

  [[nodiscard]]
  information                information         () const
  {
    MPI_Info result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_info, (native_, &result))
    return mpi::information(result);
  }
  void                       set_information     (const mpi::information& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_info, (native_, value.native()))
  }

  [[nodiscard]]
  communicator_error_handler error_handler       () const
  {
    MPI_Errhandler result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_errhandler, (native_, &result))
    return communicator_error_handler(result);
  }
  void                       set_error_handler   (const communicator_error_handler& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_errhandler, (native_, value.native()))
  }
  void                       call_error_handler  (const error_code& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_call_errhandler, (native_, value.native()))
  }

  template <typename type>
  std::optional<type>        attribute           (const communicator_key_value& key) const
  {
    type         result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_attr   , (native_, key.native(), static_cast<void*>(&result), &exists))
    return static_cast<bool>(exists) ? result : std::optional<type>(std::nullopt);
  }
  template <typename type>
  void                       set_attribute       (const communicator_key_value& key, const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_set_attr   , (native_, key.native(), static_cast<void*>(&value)))
  }
  void                       remove_attribute    (const communicator_key_value& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_delete_attr, (native_, key.native()))
  }

  void                       abort               (const error_code& error_code) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Abort, (native_, error_code.native()))
  }
  void                       barrier             () const
  {
    MPI_CHECK_ERROR_CODE(MPI_Barrier, (native_))
  }
  // Disconnect must currently be called explicitly. It is possible to state keep whether the accept/connect constructor has been used and call disconnect in the destructor if so.
  void                       disconnect          ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_disconnect, (&native_))
  }

  [[nodiscard]]
  bool                       managed             () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Comm                   native              () const
  {
    return native_;
  }

protected:
  communicator() = default; // Default constructor is only available to sub classes who control the member variables explicitly.
  
  bool     managed_ = false;
  MPI_Comm native_  = MPI_COMM_NULL;
};

inline const communicator world_communicator(MPI_COMM_WORLD);
inline const communicator self_communicator (MPI_COMM_SELF );
}