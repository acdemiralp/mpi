#pragma once

#include <string>

#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class port
{
public:
  explicit port  (const information& info = information())
  : name_(MPI_MAX_PORT_NAME, ' ')
  {
    MPI_Open_port(info.native(), &name_[0]);
  }
  port           (const port&  that) = delete;
  port           (      port&& temp) noexcept
  : name_(std::move(temp.name_))
  {
    temp.name_ = std::string();
  }
  virtual ~port  ()
  {
    if (!name_.empty())
      MPI_Close_port(name_.c_str());
  }
  port& operator=(const port&  that) = delete;
  port& operator=(      port&& temp) noexcept
  {
    if (this != &temp)
    {
      if (!name_.empty())
        MPI_Close_port(name_.c_str());

      name_      = std::move(temp.name_);
      temp.name_ = std::string();
    }
    return *this;
  }

  [[nodiscard]]
  const std::string& name() const
  {
    return name_;
  }

protected:
  std::string name_ = std::string();
};
}