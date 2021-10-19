#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

#include <mpi/core/enums/profiling_level.hpp>
#include <mpi/core/enums/thread_support.hpp>
#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/utility/sequential_container_traits.hpp>
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
  virtual ~environment  ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Finalize, ())
  }
  environment& operator=(const environment&  that)          = delete;
  environment& operator=(      environment&& temp) noexcept = delete;
};

inline bool                           initialized         ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Initialized   , (&result))
  return static_cast<bool>(result);
}
inline bool                           finalized           ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Finalized     , (&result))
  return static_cast<bool>(result);
}

inline thread_support                 query_thread_support()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Query_thread  , (&result))
  return static_cast<thread_support>(result);
}
inline bool                           is_thread_main      ()
{
  std::int32_t result;
  MPI_CHECK_ERROR_CODE(MPI_Is_thread_main, (&result))
  return static_cast<bool>(result);
}

inline std::string                    processor_name      ()
{
  std::string  result(MPI_MAX_PROCESSOR_NAME, ' ');
  std::int32_t size  (0);
  MPI_CHECK_ERROR_CODE(MPI_Get_processor_name, (&result[0], &size))
  result.resize(static_cast<std::size_t>(size));
  return result;
}

inline void                           attach_buffer       (void* buffer, const std::int32_t size)
{
  MPI_CHECK_ERROR_CODE(MPI_Buffer_attach, (buffer, size))
}
template <sequential_container type>
void                                  attach_buffer       (const type& buffer)
{
  using adapter = container_adapter<type>;
  MPI_CHECK_ERROR_CODE(MPI_Buffer_attach, (adapter::data(buffer), static_cast<std::int32_t>(adapter::size(buffer) * sizeof adapter::data_type().size())))
}
inline std::span<std::byte>           detach_buffer       ()
{
  void*        buffer;
  std::int32_t size  ;
  MPI_CHECK_ERROR_CODE(MPI_Buffer_detach, (&buffer, &size))
  return { static_cast<std::byte*>(buffer), static_cast<std::size_t>(size) };
}

inline void                           set_profiling_level (const profiling_level level)
{
  MPI_CHECK_ERROR_CODE(MPI_Pcontrol, (static_cast<std::int32_t>(level)))
}
}