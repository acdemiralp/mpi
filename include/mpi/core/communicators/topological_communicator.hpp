#pragma once

#include <cstdint>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/error/error_code.hpp>
#include <mpi/core/utility/container_adapter.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/mpi.hpp>
#include <mpi/core/request.hpp>

namespace mpi
{
class topological_communicator : public communicator
{
public:
  explicit topological_communicator   (const MPI_Comm native, const bool managed = false)
  : communicator(native, managed)
  {

  }
  topological_communicator            (const topological_communicator&  that)          = default;
  topological_communicator            (      topological_communicator&& temp) noexcept = default;
 ~topological_communicator            ()                                      override = default;
  topological_communicator& operator= (const topological_communicator&  that)          = default;
  topological_communicator& operator= (      topological_communicator&& temp) noexcept = default;
  
  [[nodiscard]] 
  virtual std::int32_t incoming_neighbor_count() const = 0;
  [[nodiscard]]
  virtual std::int32_t outgoing_neighbor_count() const = 0;

  // All to all neighborhood collective operations.
  void    neighbor_all_to_all                   (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                 sent_data_type     ,
                                                       void*          received, const std::int32_t               received_size ,                                                          const data_type&                 received_data_type ) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoall, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void    neighbor_all_to_all                   (const sent_type&     sent    , 
                                                       received_type& received) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    neighbor_all_to_all(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter   ::size(sent    ) / outgoing_neighbor_count()), send_adapter   ::data_type(), 
      receive_adapter::data(received), static_cast<std::int32_t>(receive_adapter::size(received) / incoming_neighbor_count()), receive_adapter::data_type());
  }
  [[nodiscard]]                                                           
  request immediate_neighbor_all_to_all         (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                 sent_data_type     ,
                                                       void*          received, const std::int32_t               received_size ,                                                          const data_type&                 received_data_type ) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_alltoall, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request immediate_neighbor_all_to_all         (const sent_type&     sent    , 
                                                       received_type& received) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_neighbor_all_to_all(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter   ::size(sent    ) / outgoing_neighbor_count()), send_adapter   ::data_type(), 
      receive_adapter::data(received), static_cast<std::int32_t>(receive_adapter::size(received) / incoming_neighbor_count()), receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request persistent_neighbor_all_to_all        (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                 sent_data_type     ,
                                                       void*          received, const std::int32_t               received_size ,                                                          const data_type&                 received_data_type , 
                                                 const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoall_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request persistent_neighbor_all_to_all        (const sent_type&     sent    , 
                                                       received_type& received, 
                                                 const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_neighbor_all_to_all(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter   ::size(sent    ) / outgoing_neighbor_count()), send_adapter   ::data_type(), 
      receive_adapter::data(received), static_cast<std::int32_t>(receive_adapter::size(received) / incoming_neighbor_count()), receive_adapter::data_type(), info);
  }
#endif

  void    neighbor_all_to_all_varying           (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type&                 sent_data_type     ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type&                 received_data_type ) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallv, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                                  received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void    neighbor_all_to_all_varying           (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, 
                                                 const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    neighbor_all_to_all_varying(
      send_adapter   ::data(sent    ), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      receive_adapter::data(received), received_sizes, received_displacements, receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>                            
  void    neighbor_all_to_all_varying           (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, 
                                                 const bool resize = false) const
  {
    std::vector<std::int32_t> sent_displacements    (sent_sizes    .size());
    std::vector<std::int32_t> received_displacements(received_sizes.size());
    std::exclusive_scan(sent_sizes    .begin(), sent_sizes    .end(), sent_displacements    .begin(), 0);
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), received_displacements.begin(), 0);

    neighbor_all_to_all_varying(sent, sent_sizes, sent_displacements, received, received_sizes, received_displacements, resize);
  }
  template <typename sent_type, typename received_type>                            
  void    neighbor_all_to_all_varying           (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , 
                                                       received_type& received, 
                                                 const bool resize = false) const
  {
    std::vector<std::int32_t> received_sizes(incoming_neighbor_count());
    neighbor_all_to_all(sent_sizes, received_sizes);

    neighbor_all_to_all_varying(sent, sent_sizes, received, received_sizes, resize);
  }
  [[nodiscard]]
  request immediate_neighbor_all_to_all_varying (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type&                 sent_data_type     ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type&                 received_data_type ) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_alltoallv, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                                   received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_, &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request immediate_neighbor_all_to_all_varying (const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, 
                                                 const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return immediate_neighbor_all_to_all_varying(
      send_adapter   ::data(sent    ), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      receive_adapter::data(received), received_sizes, received_displacements, receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request persistent_neighbor_all_to_all_varying(const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , const data_type&                 sent_data_type     ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, const data_type&                 received_data_type , 
                                                 const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallv_init, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_type    .native(), 
                                                       received, received_sizes.data(), received_displacements.data(), received_data_type.native(), native_, info.native(), &result.native_))
    return result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                           
  request persistent_neighbor_all_to_all_varying(const sent_type&     sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<std::int32_t>& sent_displacements    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& received_displacements, 
                                                 const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return persistent_neighbor_all_to_all_varying(
      send_adapter   ::data(sent    ), sent_sizes    , sent_displacements    , send_adapter   ::data_type(), 
      receive_adapter::data(received), received_sizes, received_displacements, receive_adapter::data_type(), info);
  }
#endif

  // The alltoallw is the only family of functions that do not have convenience wrappers (because it just cannot be made convenient).
  void    neighbor_all_to_all_general           (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<aint>&         sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<aint>&         received_displacements, const std::vector<MPI_Datatype>& received_data_types) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallw     , (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                                       received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_))
  }
  [[nodiscard]]
  request immediate_neighbor_all_to_all_general (const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<aint>&         sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<aint>&         received_displacements, const std::vector<MPI_Datatype>& received_data_types) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_alltoallw    , (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                                       received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_,                &result.native_))
    return result;
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request persistent_neighbor_all_to_all_general(const void*          sent    , const std::vector<std::int32_t>& sent_sizes    , const std::vector<aint>&         sent_displacements    , const std::vector<MPI_Datatype>& sent_data_types    ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<aint>&         received_displacements, const std::vector<MPI_Datatype>& received_data_types, 
                                                 const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_alltoallw_init, (sent    , sent_sizes    .data(), sent_displacements    .data(), sent_data_types    .data(), 
                                                       received, received_sizes.data(), received_displacements.data(), received_data_types.data(), native_, info.native(), &result.native_))
    return result;
  }
#endif

  void    neighbor_all_gather                   (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                 sent_data_type     ,
                                                       void*          received, const std::int32_t               received_size ,                                                          const data_type&                 received_data_type ) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>                            
  void    neighbor_all_gather                   (const sent_type&     sent    , 
                                                       received_type& received) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    neighbor_all_gather(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter   ::size(sent)                                ), send_adapter   ::data_type(), 
      receive_adapter::data(received), static_cast<std::int32_t>(receive_adapter::size(received) / incoming_neighbor_count()), receive_adapter::data_type());
  }
  [[nodiscard]]                                                           
  request immediate_neighbor_all_gather         (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                 sent_data_type     ,
                                                       void*          received, const std::int32_t               received_size ,                                                          const data_type&                 received_data_type ) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_allgather, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request immediate_neighbor_all_gather         (const sent_type&     sent    , 
                                                       received_type& received) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return immediate_neighbor_all_gather(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter   ::size(sent)                                ), send_adapter   ::data_type(), 
      receive_adapter::data(received), static_cast<std::int32_t>(receive_adapter::size(received) / incoming_neighbor_count()), receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]                                                           
  request persistent_neighbor_all_gather        (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                 sent_data_type     ,
                                                       void*          received, const std::int32_t               received_size ,                                                          const data_type&                 received_data_type , 
                                                 const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgather_init, (sent, sent_size, sent_data_type.native(), received, received_size, received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]                                                           
  request persistent_neighbor_all_gather        (const sent_type&     sent    , 
                                                       received_type& received, 
                                                 const mpi::information& info = mpi::information()) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;
    return persistent_neighbor_all_gather(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter   ::size(sent)                                ), send_adapter   ::data_type(), 
      receive_adapter::data(received), static_cast<std::int32_t>(receive_adapter::size(received) / incoming_neighbor_count()), receive_adapter::data_type(), info);
  }
#endif

  void    neighbor_all_gather_varying           (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                 sent_data_type     ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,          const data_type&                 received_data_type ) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_))
  }
  template <typename sent_type, typename received_type>
  void    neighbor_all_gather_varying           (const sent_type&     sent    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, 
                                                 const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    neighbor_all_gather_varying(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      receive_adapter::data(received), received_sizes, displacements                      , receive_adapter::data_type());
  }
  template <typename sent_type, typename received_type>
  void    neighbor_all_gather_varying           (const sent_type&     sent    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, 
                                                 const bool resize = false) const
  {
    std::vector<std::int32_t> displacements(received_sizes.size());
    std::exclusive_scan(received_sizes.begin(), received_sizes.end(), displacements.begin(), 0);

    neighbor_all_gather_varying(sent, received, received_sizes, displacements, resize);
  }
  template <typename sent_type, typename received_type>
  void    neighbor_all_gather_varying           (const sent_type&     sent    , 
                                                       received_type& received, 
                                                 const bool resize = false) const
  {
    std::int32_t              local_size    (container_adapter<sent_type>::size(sent));
    std::vector<std::int32_t> received_sizes(incoming_neighbor_count());
    neighbor_all_gather(local_size, received_sizes);

    neighbor_all_gather_varying(sent, received, received_sizes, resize);
  }
  [[nodiscard]]
  request immediate_neighbor_all_gather_varying (const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                sent_data_type      ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,          const data_type&                received_data_type  ) const
  {
    request result(MPI_REQUEST_NULL, true);
    MPI_CHECK_ERROR_CODE(MPI_Ineighbor_allgatherv, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_, &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]
  request immediate_neighbor_all_gather_varying (const sent_type&     sent    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements, 
                                                 const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return immediate_neighbor_all_gather_varying(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      receive_adapter::data(received), received_sizes, displacements                      , receive_adapter::data_type());
  }
#ifdef MPI_USE_LATEST
  [[nodiscard]]
  request persistent_neighbor_all_gather_varying(const void*          sent    , const std::int32_t               sent_size     ,                                                          const data_type&                sent_data_type      ,
                                                       void*          received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,          const data_type&                received_data_type  , 
                                                 const mpi::information& info = mpi::information()) const
  {
    request result(MPI_REQUEST_NULL, true, true);
    MPI_CHECK_ERROR_CODE(MPI_Neighbor_allgatherv_init, (sent, sent_size, sent_data_type.native(), received, received_sizes.data(), displacements.data(), received_data_type.native(), native_, info.native(), &result.native_))
    return  result;
  }
  template <typename sent_type, typename received_type> [[nodiscard]]
  request persistent_neighbor_all_gather_varying(const sent_type&     sent    , 
                                                       received_type& received, const std::vector<std::int32_t>& received_sizes, const std::vector<std::int32_t>& displacements,
                                                 const mpi::information& info = mpi::information(), const bool resize = false) const
  {
    using send_adapter    = container_adapter<sent_type>;
    using receive_adapter = container_adapter<received_type>;

    if (resize)
      receive_adapter::resize(received, std::reduce(received_sizes.begin(), received_sizes.end()));

    return persistent_neighbor_all_gather_varying(
      send_adapter   ::data(sent    ), static_cast<std::int32_t>(send_adapter::size(sent)), send_adapter   ::data_type(), 
      receive_adapter::data(received), received_sizes, displacements                      , receive_adapter::data_type(), info);
  }
#endif
};
}