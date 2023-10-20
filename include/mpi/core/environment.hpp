#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include <mpi/core/enums/profiling_level.hpp>
#include <mpi/core/enums/thread_support.hpp>
#include <mpi/core/structs/overhead_type.hpp>
#include <mpi/core/type/compliant_traits.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
class environment
{
public:
  explicit environment  (std::int32_t* argc = nullptr, char*** argv = nullptr)
  {
    MPI_CHECK_ERROR_CODE(MPI_Init, (argc, argv))
  }
  environment           (std::int32_t* argc          , char*** argv          , thread_support required_thread_support)
  {
    std::int32_t provided_thread_support; // Unused. Call query_thread_support() explicitly.
    MPI_CHECK_ERROR_CODE(MPI_Init_thread, (argc, argv, static_cast<std::int32_t>(required_thread_support), &provided_thread_support))
  }
  environment           (const environment&  that)          = delete;
  environment           (      environment&& temp) noexcept = delete;
  virtual ~environment  () noexcept(false)
  {
    MPI_CHECK_ERROR_CODE(MPI_Finalize, ())
  }
  environment& operator=(const environment&  that)          = delete;
  environment& operator=(      environment&& temp) noexcept = delete;
};

inline bool                 initialized         ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Initialized   , (&result))
  return static_cast<bool>(result);
}
inline bool                 finalized           ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Finalized     , (&result))
  return static_cast<bool>(result);
}

inline thread_support       query_thread_support()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Query_thread  , (&result))
  return static_cast<thread_support>(result);
}
inline bool                 is_thread_main      ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Is_thread_main, (&result))
  return static_cast<bool>(result);
}
inline std::string          processor_name      ()
{
  std::string  result(MPI_MAX_PROCESSOR_NAME, '\n');
  std::int32_t size  (0);
  MPI_CHECK_ERROR_CODE(MPI_Get_processor_name, (result.data(), &size))
  result.resize(static_cast<std::size_t>(size));
  return result;
}

inline void                 set_profiling_level (const profiling_level level)
{
  MPI_CHECK_ERROR_CODE(MPI_Pcontrol, (static_cast<std::int32_t>(level)))
}

inline void                 attach_buffer       (const std::span<std::byte>&  buffer)
{
  MPI_CHECK_ERROR_CODE(MPI_Buffer_attach, (buffer.data(), static_cast<std::int32_t>(buffer.size())))
}
inline std::span<std::byte> detach_buffer       ()
{
  void*        buffer;
  std::int32_t size  ;
  MPI_CHECK_ERROR_CODE(MPI_Buffer_detach, (&buffer, &size))
  MPI_CHECK_UNDEFINED (MPI_Buffer_detach, size)
  return { static_cast<std::byte*>(buffer), static_cast<std::size_t>(size) };
}

// Convenience for attaching an internally managed buffer.
inline void                 attach_buffer       (const std::int32_t size)
{
  static std::vector<std::byte> buffer;
  buffer.resize(size);
  attach_buffer({buffer.data(), buffer.size()});
}
// Convenience for inferring the necessary buffer size from a set of compliant types.
template <compliant... types>
void                        attach_buffer       (const buffer_type<types...>& buffer)
{
  attach_buffer(std::span(static_cast<std::byte*>(&buffer), sizeof(buffer_type<types...>)));
}
// Convenience for attaching an internally managed buffer, inferring the necessary buffer size from a set of compliant types.
template <compliant... types>
void                        attach_buffer       ()
{
  attach_buffer(sizeof(buffer_type<types...>));
}
}