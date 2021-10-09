#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <mpi/core/error/error_code.hpp>
#include <mpi/core/enums/comparison.hpp>
#include <mpi/core/enums/topology.hpp>
#include <mpi/core/error/error_handler.hpp>
#include <mpi/core/structs/spawn_data.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/group.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/port.hpp>

namespace mpi
{
class communicator
{
public:
  explicit communicator   (const MPI_Comm       native)
  : native_(native)
  {
    
  }
  communicator            (const communicator&  that, const group&       group      )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create       , (that.native_, group      .native(),      &native_))
  }
  communicator            (const communicator&  that, const group&       group      , const std::int32_t tag)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_create_group , (that.native_, group      .native(), tag, &native_))
  }
  communicator            (const communicator&  that, const information& information)
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

  [[nodiscard]]
  comparison                 compare             (const communicator& that) const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_compare, (native_, that.native_, &result))
    return static_cast<comparison>(result);
  }

  // Intercommunicator functionality.
  [[nodiscard]]
  communicator&&             accept              (const port& port, const mpi::information& info, const std::int32_t root) const
  {
    communicator intercommunicator;
    intercommunicator.managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Comm_accept , (port.name().c_str(), info.native(), root, native_, &intercommunicator.native_))
    return std::move(intercommunicator);
  }
  [[nodiscard]]
  communicator&&             connect             (const port& port, const mpi::information& info, const std::int32_t root) const
  {
    communicator intercommunicator;
    intercommunicator.managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Comm_connect, (port.name().c_str(), info.native(), root, native_, &intercommunicator.native_))
    return std::move(intercommunicator);
  }

  void                       disconnect          ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Comm_disconnect, (&native_))
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

  static communicator&&      join                (const std::int32_t socket_file_descriptor)
  {
    communicator intercommunicator;
    intercommunicator.managed_ = true;
    MPI_CHECK_ERROR_CODE(MPI_Comm_join, (socket_file_descriptor, &intercommunicator.native_))
    return std::move(intercommunicator);
  }

  // Spawn functionality.
  [[nodiscard]]
  communicator&&             spawn               (const spawn_data&              spawn_data, const std::int32_t root, const bool check_error_codes = true) const
  {
    communicator intercommunicator;
    intercommunicator.managed_ = true;

    std::vector<char*> arguments(spawn_data.arguments.size());
    std::transform(spawn_data.arguments.begin(), spawn_data.arguments.end(), arguments.begin(), [ ] (const std::string& value)
    {
      return value.c_str();
    });

    std::vector<std::int32_t> error_codes(spawn_data.process_count);

    MPI_CHECK_ERROR_CODE(MPI_Comm_spawn, (spawn_data.command.c_str(), arguments.data(), spawn_data.process_count, spawn_data.information.native(), root, native_, &intercommunicator.native_, error_codes.data()))

    if (check_error_codes)
      for (auto& code : error_codes)
        if (code != MPI_SUCCESS)
          throw exception("spawn", error_code(code));

    return std::move(intercommunicator);
  }
  [[nodiscard]]
  communicator&&             spawn_multiple      (const std::vector<spawn_data>& spawn_data, const std::int32_t root, const bool check_error_codes = true) const
  {
    communicator intercommunicator;
    intercommunicator.managed_ = true;

    std::vector<char*>              commands      (spawn_data.size());
    std::vector<std::vector<char*>> arguments     (spawn_data.size());
    std::vector<char**>             raw_arguments (spawn_data.size());
    std::vector<std::int32_t>       process_counts(spawn_data.size());
    std::vector<MPI_Info>           infos         (spawn_data.size());
    std::int32_t                    total_count   (0);
    for (std::size_t i = 0; i < spawn_data.size(); ++i)
    {
      arguments[i].resize(spawn_data[i].arguments.size());
      std::transform(spawn_data[i].arguments.begin(), spawn_data[i].arguments.end(), arguments[i].begin(), [ ] (const std::string& value)
      {
        return value.c_str();
      });

      commands      [i]  = const_cast<char*>(spawn_data[i].command.c_str());
      raw_arguments [i]  = arguments.back().data();
      process_counts[i]  = spawn_data[i].process_count;
      infos         [i]  = spawn_data[i].information.native();
      total_count       += spawn_data[i].process_count;
    }

    std::vector<std::int32_t> error_codes(total_count);

    MPI_CHECK_ERROR_CODE(MPI_Comm_spawn_multiple, (static_cast<std::int32_t>(spawn_data.size()), commands.data(), raw_arguments.data(), process_counts.data(), infos.data(), root, native_, &intercommunicator.native_, error_codes.data()))

    if (check_error_codes)
      for (auto& code : error_codes)
        if (code != MPI_SUCCESS)
          throw exception("spawn_multiple", error_code(code));

    return std::move(intercommunicator);
  }
  [[nodiscard]]
  communicator&&             parent              () const
  {
    MPI_Comm result;
    MPI_CHECK_ERROR_CODE(MPI_Comm_get_parent, (&result))
    return std::move(communicator(result));
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