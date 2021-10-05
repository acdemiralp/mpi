#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
// Standard:
// "... It stores an unordered set of (key, value) pairs (both key and value are strings). A key can have only one value. ..."
//
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
  explicit information   (const map_type&     map   )
  : information()
  {
    for (const auto& [key, value] : map)
      emplace(key, value);
  }
  explicit information   (const MPI_Info      native)
  : native_(native)
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
  virtual ~information   ()
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
  information& operator= (      information&& temp  ) noexcept
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

  std::string  operator[](const std::string&  key   ) const
  {
    return at(key);
  }
  std::string  operator[](const std::int32_t  index ) const
  {
    return at(key_at(index));
  }

  [[nodiscard]]                                
  bool         contains(const std::string& key  ) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_valuelen, (native_, key.c_str(), &size, &exists))

    return static_cast<bool>(exists);
  }
  [[nodiscard]]
  std::string  at      (const std::string& key  ) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_valuelen, (native_, key.c_str(), &size, &exists))

    std::string result(size, ' ');
    MPI_CHECK_ERROR_CODE(MPI_Info_get, (native_, key.c_str(), size, &result[0], &exists))

    return result;
  }
  [[nodiscard]]                                
  std::string  key_at  (const std::int32_t index) const
  {
    std::string result(MPI_MAX_INFO_KEY, ' ');
    MPI_CHECK_ERROR_CODE(MPI_Info_get_nthkey, (native_, index, &result[0]))
    return result;
  }

  [[nodiscard]]                                
  std::int32_t size    () const
  {
    auto result(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_nkeys, (native_, &result))
    return result;
  }
  [[nodiscard]]                                
  bool         empty   () const
  {
    return size() == 0;
  }

  void         emplace (const std::string& key, const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_set   , (native_, key.c_str(), value.c_str()))
  }
  void         erase   (const std::string& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_delete, (native_, key.c_str()))
  }
  void         clear   () const
  {
    while (size() > 0)
      erase(key_at(0));
  }

  [[nodiscard]]
  map_type     map     () const
  {
    map_type result;
    for (auto i = 0; i < size(); ++i)
    {
      const auto key = key_at(i);
      result.emplace(key, at(key));
    }
    return result;
  }

  [[nodiscard]]
  bool         managed () const
  {
    return managed_;
  }
  [[nodiscard]]                                 
  MPI_Info     native  () const
  {
    return native_;
  }

protected:
  bool     managed_ = false;
  MPI_Info native_  = MPI_INFO_NULL;
};

inline const information environment_information { MPI_INFO_ENV };
}