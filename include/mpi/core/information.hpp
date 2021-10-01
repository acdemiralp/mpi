#pragma once

#include <cstdint>
#include <string>

#include <mpi/core/mpi.hpp>

namespace mpi
{
class information
{
public:
  information            ()
  : managed_(true)
  {
    MPI_Info_create(&native_);
  }
  explicit information   (const MPI_Info native)
  : native_(native)
  {

  }
  information            (const information&  that)
  : managed_(true)
  {
    MPI_Info_dup(that.native_, &native_);
  }
  information            (      information&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = 0;
  }
  virtual ~information   ()
  {
    if (managed_)
      MPI_Info_free(&native_);
  }
  information& operator= (const information&  that)
  {
    managed_ = true;
    MPI_Info_dup(that.native_, &native_);
    return *this;
  }
  information& operator= (      information&& temp) noexcept
  {
    if (this != &temp)
    {
      managed_      = temp.managed_;
      native_       = temp.native_;
      temp.managed_ = false;
      temp.native_  = 0;
    }
    return *this;
  }

  std::string  operator[](const std::int32_t index) const
  {
    return at(index);
  }
  std::string  operator[](const std::string& key  ) const
  {
    return at(key);
  }

  [[nodiscard]]
  std::int32_t size   () const
  {
    auto result(0);
    MPI_Info_get_nkeys(native_, &result);
    return result;
  }
  [[nodiscard]]
  std::string  at     (const std::int32_t index) const
  {
    std::string result(MPI_MAX_INFO_KEY, ' ');
    MPI_Info_get_nthkey(native_, index, &result[0]);
    return result;
  }
  [[nodiscard]]
  std::string  at     (const std::string& key) const
  {
    auto size(0), exists(0);
    MPI_Info_get_valuelen(native_, key.c_str(), &size, &exists);

    std::string result(size, ' ');
    MPI_Info_get(native_, key.c_str(), size, &result[0], &exists);

    return result;
  }
  [[nodiscard]]
  bool         set    (const std::string& key, const std::string& value) const
  {
    return MPI_Info_set(native_, key.c_str(), value.c_str()) == MPI_SUCCESS;
  }
  [[nodiscard]]
  bool         remove (const std::string& key) const
  {
    return MPI_Info_delete(native_, key.c_str()) == MPI_SUCCESS;
  }

  [[nodiscard]]
  bool         managed() const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Info     native () const
  {
    return native_;
  }

protected:
  bool     managed_ = false;
  MPI_Info native_  = 0;
};

inline const information& environment_information()
{
  static const information info(MPI_INFO_ENV);
  return info;
}
}