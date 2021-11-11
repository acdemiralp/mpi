#pragma once

#include <cstdint>

#include <mpi/core/utility/container_adapter.hpp>

namespace mpi
{
// The type must satisfy compliant.
template <typename type>
class sync_variable
{
public:
  explicit sync_variable  (const communicator& communicator, const std::int32_t root = 0)
  : communicator_(communicator), window_(communicator, sizeof type), root_(root)
  {

  }
  sync_variable           (const sync_variable&  that) = delete ;
  sync_variable           (      sync_variable&& temp) = default;
  virtual ~sync_variable  ()                           = default;
  sync_variable& operator=(const sync_variable&  that) = delete ;
  sync_variable& operator=(      sync_variable&& temp) = default;

  void put(const type& value)
  {
    window_.lock  (root_, false);
    window_.put   (value , root_, 0, 1, container_adapter<type>::data_type());
    window_.unlock(root_);
  }                
  [[nodiscard]]    
  type get() const 
  {               
    type result;
    window_.lock  (root_, false);
    window_.get   (result, root_, 0, 1, container_adapter<type>::data_type());
    window_.unlock(root_);
    return result;
  }

  void synchronize() const
  {
    window_.fence();
  }

protected:
  const communicator& communicator_;
  window              window_      ;
  std::int32_t        root_        ;
};
}