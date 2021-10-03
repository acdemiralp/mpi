#pragma once

#include <string>
#include <utility>

#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/port.hpp>

namespace mpi
{
class service
{
public:
  service           (const std::string& name, const port* port, const information& info = information())
  : name_(name), info_(info), port_(port)
  {
    MPI_Publish_name  (name_.c_str(), info_.native(), port_->name().c_str());
  }
  service           (const service&  that) = delete;
  service           (      service&& temp) noexcept
  : name_(std::move(temp.name_)), info_(std::move(temp.info_)), port_(temp.port_)
  {
    temp.name_ = std::string();
    temp.info_ = information();
    temp.port_ = nullptr;
  }
  virtual ~service  ()
  {
    if (!name_.empty())
      MPI_Unpublish_name(name_.c_str(), info_.native(), port_->name().c_str());
  }
  service& operator=(const service&  that) = delete;
  service& operator=(      service&& temp) noexcept
  {
    if (this != &temp)
    {
      name_      = std::move(temp.name_);
      info_      = std::move(temp.info_);
      port_      = temp.port_;

      temp.name_ = std::string();
      temp.info_ = information();
      temp.port_ = nullptr;
    }
    return *this;
  }

  static port look_up(const std::string& name, const information& info = information())
  {
    std::string result(MPI_MAX_PORT_NAME, ' ');
    MPI_Lookup_name(name.c_str(), info.native(), &result[0]);
    return port(result);
  }

protected:
  std::string name_;
  information info_;
  const port* port_;
};
}