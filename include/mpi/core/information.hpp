#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
namespace io
{
class file;
}
namespace tool
{
class event_handle;
}

// Standard: "... It stores an unordered set of (key, value) pairs (both key and value are strings). A key can have only one value. ..."
// This interface is loosely based on associative containers such as std::unordered_map (it is not STL compliant).
// It can additionally be converted from/to std::unordered_map<std::string, std::string>.
class information
{
public:
  using map_type = std::unordered_map<std::string, std::string>;

  information            ()
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_create, (&native_))
  }
#ifdef MPI_GEQ_4_0
  information            (std::int32_t argc, char** argv)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_create_env, (argc, argv, &native_))
  }
#endif
  explicit information   (const map_type&     map   )
  : information()
  {
    for (const auto& [key, value] : map)
      emplace(key, value);
  }
  explicit information   (const MPI_Info      native, const bool managed = false)
  : managed_(managed), native_(native)
  {

  }
  information            (const information&  that  )
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_dup, (that.native_, &native_))
  }
  information            (      information&& temp  ) noexcept
  : managed_(temp.managed_), native_ (temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_INFO_NULL;
  }
  virtual ~information   () noexcept(false)
  {
    if (managed_ && native_ != MPI_INFO_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Info_free, (&native_))
  }
  information& operator= (const information&  that  )
  {
    if (this != &that)
    {
      if (managed_ && native_ != MPI_INFO_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Info_free, (&native_))

      managed_ = true;
      MPI_CHECK_ERROR_CODE(MPI_Info_dup, (that.native_, &native_))
    }
    return *this;
  }
  information& operator= (      information&& temp  ) noexcept(false)
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_INFO_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Info_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_INFO_NULL;
    }
    return *this;
  }

  std::optional<std::string> operator[](const std::string& key) const
  {
    return at(key);
  }
  std::optional<std::string> operator[](const std::int32_t index) const
  {
    return at(key_at(index));
  }

  [[nodiscard]]                                
  std::int32_t               size    () const
  {
    auto result(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_nkeys, (native_, &result))
    return result;
  }
  [[nodiscard]]                                
  bool                       empty   () const
  {
    return size() == 0;
  }
  
#ifdef MPI_GEQ_4_0
  [[nodiscard]]                                
  bool                       contains(const std::string& key  ) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_string, (native_, key.c_str(), &size, nullptr, &exists))

    return static_cast<bool>(exists);
  }
  [[nodiscard]]
  std::optional<std::string> at      (const std::string& key  ) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_string, (native_, key.c_str(), &size, nullptr, &exists))

    if (!static_cast<bool>(exists))
      return {std::nullopt};

    std::string result(size, '\n');
    MPI_CHECK_ERROR_CODE(MPI_Info_get_string, (native_, key.c_str(), &size, &result[0], &exists))

    return result;
  }
#else
  [[nodiscard]]                                
  bool                       contains(const std::string& key  ) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_valuelen, (native_, key.c_str(), &size, &exists))

    return static_cast<bool>(exists);
  }
  [[nodiscard]]
  std::optional<std::string> at      (const std::string& key  ) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_valuelen, (native_, key.c_str(), &size, &exists))

    if (!static_cast<bool>(exists))
      return {std::nullopt};

    std::string result(size, '\n');
    MPI_CHECK_ERROR_CODE(MPI_Info_get, (native_, key.c_str(), size, result.data(), &exists))

    return result;
  }
#endif
  [[nodiscard]]                                
  std::string                key_at  (const std::int32_t index) const
  {
    std::string result(MPI_MAX_INFO_KEY, '\n');
    MPI_CHECK_ERROR_CODE(MPI_Info_get_nthkey, (native_, index, result.data()))
    return result;
  }
  [[nodiscard]]
  map_type                   map     () const
  {
    map_type result;
    for (auto i = 0; i < size(); ++i)
    {
      const auto key = key_at(i);
      result.emplace(key, *at(key));
    }
    return result;
  }

  void                       emplace (const std::string& key, const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_set   , (native_, key.c_str(), value.c_str()))
  }
  void                       erase   (const std::string& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_delete, (native_, key.c_str()))
  }
  void                       clear   () const
  {
    while (size() > 0)
      erase(key_at(0));
  }

  [[nodiscard]]
  bool                       managed () const
  {
    return managed_;
  }
  [[nodiscard]]                                 
  MPI_Info                   native  () const
  {
    return native_;
  }

protected:
  friend class communicator;
  friend class session;
  friend class window;
  friend class io::file;
  friend class tool::event_handle;

  bool     managed_ = false;
  MPI_Info native_  = MPI_INFO_NULL;
};

inline const information environment_information { MPI_INFO_ENV };
}