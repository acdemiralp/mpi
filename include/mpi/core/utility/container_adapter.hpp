#pragma once

#include <type_traits>
#include <valarray>
#include <vector>

#include <mpi/core/type/compliant_container_traits.hpp>
#include <mpi/core/type/type_traits.hpp>

// Old Solution:
//
// - For basic compliant types:
//   - Exposes .size() = 1 and .data() = &object for read and write operations.
// - For compliant associative containers and non contiguous sequential containers:
//   - Copies the contents of the container to a vector<container::value_type> using .begin() and .end().
//   - Exposes .size() and .data() of the vector for read and write operations.
// - For compliant contiguous sequential containers:
//   - Exposes .size() and .data() of the container for read and write operations.
//
// Before proceeding on a solution, it is necessary to understand how adapters are used.
//
// What are container adapters?
// - Container adapters are an abstraction to provide a uniform interface for accessing and mutating:
//   - Compliant types.
//   - Associative containers.
//   - Non-contiguous sequential containers.
//   - Sequential containers.
//
// Why are container adapters used?
// - Send and receive, as well as most collectives accept data using the signature `(const) void* buffer, std::int32_t count, MPI_Datatype data_type`
//   where the buffer is a C pointer or array to `count` many objects of type `data_type`.
//   Container adapters unify the interface to obtain the `buffer`, `count` and `data_type` of single objects and containers.
// - Associative and non-contiguous sequential containers do not guarantee contiguous memory but `buffer` is expected to be contiguous.
//   Container adapters copy these containers to a contiguous std::vector<value_type> and provide the `buffer` as a pointer to this vector.
//
// Where are container adapters used?
// - Point-to-Point Communication:
//   - MPI_Send    , MPI_Bsend , MPI_Rsend , MPI_Ssend
//   - MPI_Isend   , MPI_Ibsend, MPI_Irsend, MPI_Issend
//   - MPI_Recv    , MPI_Mrecv
//   - MPI_Irecv   , MPI_Imrecv
//   - MPI_Sendrecv, MPI_Sendrecv_replace
// - Collectives:
//   - MPI_Bcast              , MPI_Gather              , MPI_Gatherv        , MPI_Scatter              , MPI_Scatterv           , MPI_Scan , MPI_Exscan 
//   - MPI_Reduce             , MPI_Reduce_local        , MPI_Reduce_scatter , MPI_Reduce_scatter_block
//   - MPI_Allgather          , MPI_Allgatherv          , MPI_Allreduce      , MPI_Alltoall             , MPI_Alltoallv          , MPI_Alltoallw
//   - MPI_Neighbor_allgather , MPI_Neighbor_allgatherv                      , MPI_Neighbor_alltoall    , MPI_Neighbor_alltoallv , MPI_Neighbor_alltoallw
//   - MPI_Ibcast             , MPI_Igather             , MPI_Igatherv       , MPI_Iscatter             , MPI_Iscatterv          , MPI_Iscan, MPI_Iexscan
//   - MPI_Ireduce                                      , MPI_Ireduce_scatter, MPI_Ireduce_scatter_block 
//   - MPI_Iallgather         , MPI_Iallgatherv         , MPI_Iallreduce     , MPI_Ialltoall            , MPI_Ialltoallv         , MPI_Ialltoallw
//   - MPI_Ineighbor_allgather, MPI_Ineighbor_allgatherv                     , MPI_Ineighbor_alltoall   , MPI_Ineighbor_alltoallv, MPI_Ineighbor_alltoallw
// - Packing:
//   - MPI_Pack, MPI_Pack_external, MPI_Unpack, MPI_Unpack_external
// - File:
//   - MPI_File_read         , MPI_File_read_all           , MPI_File_read_all_begin    , MPI_File_read_all_end
//   - MPI_File_write        , MPI_File_write_all          , MPI_File_write_all_begin   , MPI_File_write_all_end
//   - MPI_File_read_at      , MPI_File_read_at_all        , MPI_File_read_at_all_begin , MPI_File_read_at_all_end
//   - MPI_File_write_at     , MPI_File_write_at_all       , MPI_File_write_at_all_begin, MPI_File_write_at_all_end
//   - MPI_File_read_ordered , MPI_File_read_ordered_begin , MPI_File_read_ordered_end
//   - MPI_File_write_ordered, MPI_File_write_ordered_begin, MPI_File_write_ordered_end
//   - MPI_File_read_shared  , MPI_File_write_shared
//   - MPI_File_iread        , MPI_File_iread_all , MPI_File_iread_at , MPI_File_iread_at_all , MPI_File_iread_shared
//   - MPI_File_iwrite       , MPI_File_iwrite_all, MPI_File_iwrite_at, MPI_File_iwrite_at_all, MPI_File_iwrite_shared
// - Tool:
//  - MPI_T_cvar_read, MPI_T_cvar_write, MPI_T_pvar_read, MPI_T_pvar_readreset, MPI_T_pvar_write
//
// How are container adapters used?
// - TODO
namespace mpi
{
template <typename type, typename = void>
class container_adapter;

template <compliant type>
class container_adapter<type>
{
public:
  using value_type = type;

  explicit container_adapter  (type& container) : container_(container)
  {
    
  }
  container_adapter           (const container_adapter&  that) = delete ;
  container_adapter           (      container_adapter&& temp) = default;
  virtual ~container_adapter  ()                               = default;
  container_adapter& operator=(const container_adapter&  that) = delete ;
  container_adapter& operator=(      container_adapter&& temp) = default;

  [[nodiscard]]
  std::size_t size  () const
  {
    return 1;
  }
  [[nodiscard]]
  value_type* data  ()
  {
    return &container_;
  }

  void        resize(const std::size_t size)
  {
    // Do nothing. Compliant types are not resizable.
  }

protected:
  type& container_;
};

// TODO: Persistence of adapter vector across async calls. Copying back from the adapter vector to the original container.
template <compliant_associative_container type>
class container_adapter<type>
{
public:
  using value_type                = typename type::value_type;
  using contiguous_container_type = std::vector<value_type>;

  explicit container_adapter  (type& container) : container_(container), contiguous_container_(container.begin(), container.end())
  {

  }
  container_adapter           (const container_adapter&  that) = delete ;
  container_adapter           (      container_adapter&& temp) = default;
  virtual ~container_adapter  ()                               = default;
  container_adapter& operator=(const container_adapter&  that) = delete ;
  container_adapter& operator=(      container_adapter&& temp) = default;

  [[nodiscard]]
  std::size_t size  () const
  {
    return contiguous_container_.size();
  }
  [[nodiscard]]
  value_type* data  ()
  {
    return contiguous_container_.data();
  }

  void        resize(const std::size_t size)
  {
    contiguous_container_.resize(size);
  }

protected:
  type&                     container_;
  contiguous_container_type contiguous_container_;
};

// TODO: Persistence of adapter vector across async calls. Copying back from the adapter vector to the original container.
template <compliant_non_contiguous_sequential_container type>
class container_adapter<type>
{
public:
  using value_type                = typename type::value_type;
  using contiguous_container_type = std::vector<value_type>;

  explicit container_adapter  (type& container) : container_(container), contiguous_container_(container.begin(), container.end())
  {

  }
  container_adapter           (const container_adapter&  that) = delete ;
  container_adapter           (      container_adapter&& temp) = default;
  virtual ~container_adapter  ()                               = default;
  container_adapter& operator=(const container_adapter&  that) = delete ;
  container_adapter& operator=(      container_adapter&& temp) = default;

  [[nodiscard]]
  std::size_t size  () const
  {
    return contiguous_container_.size();
  }
  [[nodiscard]]
  value_type* data  ()
  {
    return contiguous_container_.data();
  }

  void        resize(const std::size_t size)
  {
    contiguous_container_.resize(size);
  }

protected:
  type&                     container_;
  contiguous_container_type contiguous_container_;
};

template <compliant_contiguous_sequential_container type>
class container_adapter<type>
{
public:
  using value_type = typename type::value_type;

  explicit container_adapter  (type& container) : container_(container)
  {
    
  }
  container_adapter           (const container_adapter&  that) = delete ;
  container_adapter           (      container_adapter&& temp) = default;
  virtual ~container_adapter  ()                               = default;
  container_adapter& operator=(const container_adapter&  that) = delete ;
  container_adapter& operator=(      container_adapter&& temp) = default;

  [[nodiscard]]
  std::size_t size  () const
  {
    return container_.size();
  }
  [[nodiscard]]
  value_type* data  ()
  {
    if constexpr (std::is_same_v<type, std::valarray<value_type>>)
      return &container_[0];  // std::valarray does not have a .data() function.
    else
      return container_.data();
  }

  void        resize(const std::size_t size)
  {
    container_.resize(size);
  }

protected:
  type& container_;
};
}