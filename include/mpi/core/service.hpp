#pragma once

#include <string>

#include <mpi/core/exception.hpp>
#include <mpi/core/information.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/port.hpp>

namespace mpi
{
class service
{
public:
  // The info argument is last in order to have a default.
  service           (const std::string& name, const port& port, const information& info = information())
  : name_(name), info_(info), port_(port)
  {
    MPI_CHECK_ERROR_CODE(MPI_Publish_name  , (name_.c_str(), info_.native(), port_.name().c_str()))
  }
  service           (const service&  that)          = delete;
  service           (      service&& temp) noexcept = delete;
  virtual ~service  () noexcept(false)
  {
    MPI_CHECK_ERROR_CODE(MPI_Unpublish_name, (name_.c_str(), info_.native(), port_.name().c_str()))
  }
  service& operator=(const service&  that)          = delete;
  service& operator=(      service&& temp) noexcept = delete;

protected:
  std::string name_;
  information info_;
  const port& port_; // The port's lifetime must outlast the service.
};

inline port lookup_service(const std::string& name, const information& info = information())
{
  std::string result(MPI_MAX_PORT_NAME, '\n');
  MPI_CHECK_ERROR_CODE(MPI_Lookup_name, (name.c_str(), info.native(), &result[0]))
  return port(result); // Unmanaged construction.
}
}