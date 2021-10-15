#pragma once

#include <type_traits>
#include <valarray>
#include <vector>

#include <mpi/core/type/compliant_container_traits.hpp>
#include <mpi/core/type/type_traits.hpp>

// Container adapter provides a uniform interface for accessing different data type categories.
//
// - For basic compliant types:
//   - Exposes .size() = 1 and .data() = &object for read and write operations.
//
// - For compliant associative containers and non contiguous sequential containers:
//   - Copies the contents of the container to a vector<container::value_type> using .begin() and .end().
//   - Exposes .size() and .data() of the vector for read and write operations.
//
// - For compliant contiguous sequential containers:
//   - Exposes .size() and .data() of the container for read and write operations.
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