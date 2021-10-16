#pragma once

#include <string>
#include <utility>

#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class port
{
public:
  explicit port  (const information& info = information())
  : managed_(true), name_(MPI_MAX_PORT_NAME, ' ')
  {
    MPI_CHECK_ERROR_CODE(MPI_Open_port, (info.native(), &name_[0]))
  }
  explicit port  (const std::string& name)
  : name_(name)
  {

  }
  port           (const port&  that) = delete;
  port           (      port&& temp) noexcept
  : managed_(temp.managed_), name_(std::move(temp.name_))
  {
    temp.managed_ = false;
    temp.name_.clear();
  }
  virtual ~port  ()
  {
    if (managed_ && !name_.empty())
      MPI_CHECK_ERROR_CODE(MPI_Close_port, (name_.c_str()))
  }
  port& operator=(const port&  that) = delete;
  port& operator=(      port&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && !name_.empty())
        MPI_CHECK_ERROR_CODE(MPI_Close_port, (name_.c_str()))

      managed_      = temp.managed_;
      name_         = std::move(temp.name_);

      temp.managed_ = false;
      temp.name_.clear();
    }
    return *this;
  }

  [[nodiscard]]
  bool               managed() const
  {
    return managed_;
  }
  [[nodiscard]]
  const std::string& name   () const
  {
    return name_;
  }

protected:
  bool        managed_ = false;
  std::string name_    = std::string();
};
}