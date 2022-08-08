#pragma once

#include <cstdint>

#include <mpi/core/communicators/communicator.hpp>
#include <mpi/core/type/type_traits.hpp>
#include <mpi/core/window.hpp>

namespace mpi
{
// The type must be compliant.
// Requires .synchronize() to be called explicitly to ensure synchronization.
template <typename type>
class manual_shared_variable
{
public:
  explicit manual_shared_variable          (const communicator& communicator, const std::int32_t root = 0)
  : communicator_(communicator), root_(root), window_(communicator_, communicator_.rank() == root_ ? type_traits<type>::get_data_type().size() : 0)
  {

  }
  manual_shared_variable                   (const manual_shared_variable&  that) = delete ;
  manual_shared_variable                   (      manual_shared_variable&& temp) = default;
  virtual ~manual_shared_variable          ()                                    = default;
  manual_shared_variable& operator=(const manual_shared_variable&  that) = delete ;
  manual_shared_variable& operator=(      manual_shared_variable&& temp) = default;
  manual_shared_variable& operator=(const type& value)
  {
    set(value);
    return *this;
  }
  operator type() const
  {
    return get();
  }

  void set        (const type& value) const
  {
    window_.lock  (root_, false);
    window_.put   (value , root_, 0, 1, type_traits<type>::get_data_type());
    window_.unlock(root_);
  }
  [[nodiscard]]
  type get        () const 
  {               
    type result {};
    window_.lock  (root_, true);
    window_.get   (result, root_, 0, 1, type_traits<type>::get_data_type());
    window_.unlock(root_);
    return result;
  }

  void synchronize() const
  {
    window_.fence();
  }

protected:
  const communicator& communicator_;
  std::int32_t        root_        ;
  window              window_      ;
};

// The type must be compliant.
// Calls .synchronize() globally after a .set_if_rank(...), ensuring synchronization.
// Note that .set_if_rank(...) must be called on all processes in the communicator as if its a collective.
template <typename type>
class automatic_shared_variable : public manual_shared_variable<type>
{
public:
  using base = manual_shared_variable<type>;

  explicit automatic_shared_variable  (const communicator& communicator, const std::int32_t root = 0)
  : base(communicator, root)
  {

  }
  automatic_shared_variable           (const automatic_shared_variable&  that) = delete ;
  automatic_shared_variable           (      automatic_shared_variable&& temp) = default;
 ~automatic_shared_variable           () override                              = default;
  automatic_shared_variable& operator=(const automatic_shared_variable&  that) = delete ;
  automatic_shared_variable& operator=(      automatic_shared_variable&& temp) = default;
  automatic_shared_variable& operator=(const type& value)                      = delete ;
  operator type() const
  {
    return base::get();
  }

  void set_if_rank(const type& value, const std::int32_t rank) const
  {
    if (base::communicator_.rank() == rank)
      base::set(value);
    base::synchronize();
  }
};

template <typename type>
using shared_variable = automatic_shared_variable<type>;
}