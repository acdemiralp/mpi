#pragma once

#include <cstdint>
#include <limits>
#include <string>

#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

#include <map>

namespace mpi
{
// Standard:
// "... It stores an unordered set of (key, value) pairs (both key and value are strings). A key can have only one value. ..."
// which means it is equivalent to a std::unordered_map<std::string, std::string>.
// 
// The interface is loosely based on associative containers such as std::unordered_map.
// Note that it is not STL compliant i.e. functions which do not have a correspondent are omitted.
class information
{
public:
  information            ()
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_create, (&native_))
  }
  explicit information   (const MPI_Info native)
  : managed_(false )
  , native_ (native)
  {

  }
  information            (const information&  that)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_dup, (that.native_, &native_))
  }
  information            (      information&& temp) noexcept
  : managed_(temp.managed_)
  , native_ (temp.native_ )
  {
    temp.managed_ = false;
    temp.native_  = MPI_INFO_NULL;
  }
  virtual ~information   ()
  {
    if (managed_ && native_ != MPI_INFO_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Info_free, (&native_))
  }
  information& operator= (const information&  that)
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
  information& operator= (      information&& temp) noexcept
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

  std::string  operator[](const std::string&  key) const
  {
    return at(key);
  }
  std::string  operator[](      std::string&& key)
  {
    return at(key);
  }

  // Element access.
  [[nodiscard]]
  std::string  at              (const std::string& key  ) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_valuelen, (native_, key.c_str(), &size, &exists))

    std::string result(size, ' ');
    MPI_CHECK_ERROR_CODE(MPI_Info_get, (native_, key.c_str(), size, &result[0], &exists))

    return result;
  }
  [[nodiscard]]
  std::string  key_at          (const std::int32_t index) const
  {
    std::string result(MPI_MAX_INFO_KEY, ' ');
    MPI_CHECK_ERROR_CODE(MPI_Info_get_nthkey, (native_, index, &result[0]))
    return result;
  }

  // Iterators.
  // TODO: cbegin/cend/crbegin/crend

  // Capacity.
  [[nodiscard]]
  bool         empty           () const
  {
    return size() == 0;
  }
  [[nodiscard]]
  std::int32_t size            () const
  {
    auto result(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_nkeys, (native_, &result))
    return result;
  }
  [[nodiscard]]
  std::int32_t max_size        () const
  {
    return std::numeric_limits<std::int32_t>::max();
  }
  
  // Modifiers.
  void         clear           () const
  {
    while (size() > 0)
      erase(key_at(0));
  }
  void         insert          (const std::pair<std::string, std::string>& kvp) const
  {
    if (!contains(kvp.first))
      emplace(kvp.first, kvp.second);
  }
  void         insert_or_assign(const std::pair<std::string, std::string>& kvp) const
  {
    
  }
  void         emplace         (const std::string& key, const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_set, (native_, key.c_str(), value.c_str()))
  }
  void         try_emplace     (const std::string& key, const std::string& value) const
  {
    
  }
  void         erase           (const std::string& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Info_delete, (native_, key.c_str()))
  }
  void         swap            (information& that) noexcept
  {

  }
  // TODO: Extract, merge.
  // TODO: Convert from/to std::map/unordered_map/set/unordered_set.

  // Lookup.
  // TODO: Find, equal_range.
  [[nodiscard]]
  bool         contains        (const std::string& key) const
  {
    return count(key) > 0;
  }
  [[nodiscard]]
  std::int32_t count           (const std::string& key) const
  {
    auto size(0), exists(0);
    MPI_CHECK_ERROR_CODE(MPI_Info_get_valuelen, (native_, key.c_str(), &size, &exists))
    return exists;
  }

  // Non-members.
  // TODO: operator==, std::swap, erase_if

  [[nodiscard]]
  bool         managed         () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Info     native          () const
  {
    return native_;
  }

protected:
  bool     managed_ = false;
  MPI_Info native_  = MPI_INFO_NULL;
};

inline const information environment_information { MPI_INFO_ENV };
}