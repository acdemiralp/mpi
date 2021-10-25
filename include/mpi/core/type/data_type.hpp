#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <mpi/core/enums/combiner.hpp>
#include <mpi/core/enums/type_class.hpp>
#include <mpi/core/structs/data_type_information.hpp>
#include <mpi/core/structs/distributed_array_information.hpp>
#include <mpi/core/structs/sub_array_information.hpp>
#include <mpi/core/exception.hpp>
#include <mpi/core/key_value.hpp>
#include <mpi/core/mpi.hpp>

namespace mpi
{
namespace io
{
class file;
}

class data_type
{
public:
  explicit data_type  (const MPI_Datatype native, const bool managed = false)
  : managed_(managed), native_(native)
  {
    
  }
  data_type           (const type_class  type, const std::int32_t size)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_match_size, (static_cast<std::int32_t>(type), size, &native_))
  }
  data_type           (const std::vector<data_type>& data_types, const std::vector<std::int32_t>& block_lengths, const std::vector<std::int64_t>& displacements)
  : managed_(true)
  {
    std::vector<MPI_Datatype> raw_data_types(data_types.size());
    std::ranges::transform(data_types, raw_data_types.begin(), [ ] (const auto& type)
    {
      return type.native();
    });

    MPI_CHECK_ERROR_CODE(MPI_Type_create_struct  , (static_cast<std::int32_t>(block_lengths.size()), block_lengths.data(), displacements.data(), raw_data_types.data(), &native_))
  }
  data_type           (const data_type&  that, const std::int32_t count)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_contiguous, (count, that.native_, &native_)) 
  }
  data_type           (const data_type&  that, const std::int64_t lower_bound, const std::int64_t extent)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_resized, (that.native_, lower_bound, extent, &native_)) 
  }
  data_type           (const data_type&  that, const std::int32_t count, const std::int32_t block_length, const std::int32_t stride)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_vector , (count, block_length, stride, that.native_, &native_)) 
  }
  data_type           (const data_type&  that, const std::int32_t count, const std::int32_t block_length, const std::int64_t stride)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_hvector, (count, block_length, stride, that.native_, &native_))
  }
  data_type           (const data_type&  that, const std::int32_t               block_length , const std::vector<std::int32_t>& displacements)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_indexed_block , (static_cast<std::int32_t>(displacements.size()), block_length, displacements.data(), that.native_, &native_))
  }
  data_type           (const data_type&  that, const std::int32_t               block_length , const std::vector<std::int64_t>& displacements)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_hindexed_block, (static_cast<std::int32_t>(displacements.size()), block_length, displacements.data(), that.native_, &native_))
  }
  data_type           (const data_type&  that, const std::vector<std::int32_t>& block_lengths, const std::vector<std::int32_t>& displacements)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_indexed        , (static_cast<std::int32_t>(block_lengths.size()), block_lengths.data(), displacements.data(), that.native_, &native_))
  }
  data_type           (const data_type&  that, const std::vector<std::int32_t>& block_lengths, const std::vector<std::int64_t>& displacements)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_hindexed, (static_cast<std::int32_t>(block_lengths.size()), block_lengths.data(), displacements.data(), that.native_, &native_))
  }
  data_type           (const data_type&  that, const sub_array_information&         sub_array)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_subarray, (
      static_cast<std::int32_t>(sub_array.sizes.size()), 
      sub_array.sizes    .data(), 
      sub_array.sub_sizes.data(),
      sub_array.starts   .data(), 
      sub_array.fortran_order ? MPI_ORDER_FORTRAN : MPI_ORDER_C, 
      that.native_, 
      &native_))
  }
  data_type           (const data_type&  that, const distributed_array_information& distributed_array)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_create_darray, (
      distributed_array.size,
      distributed_array.rank,
      static_cast<std::int32_t>            (distributed_array.global_sizes .size()),
      distributed_array.global_sizes          .data(),
      reinterpret_cast<const std::int32_t*>(distributed_array.distributions.data()),
      distributed_array.distribution_arguments.data(),
      distributed_array.process_grid_sizes    .data(),
      distributed_array.fortran_order ? MPI_ORDER_FORTRAN : MPI_ORDER_C,
      that.native_, 
      &native_))
  }
  data_type           (const data_type&  that)
  : managed_(true)
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_dup, (that.native_, &native_))
  }
  data_type           (      data_type&& temp) noexcept
  : managed_(temp.managed_), native_(temp.native_)
  {
    temp.managed_ = false;
    temp.native_  = MPI_DATATYPE_NULL;
  }
  virtual ~data_type  ()
  {
    if (managed_ && native_ != MPI_DATATYPE_NULL)
      MPI_CHECK_ERROR_CODE(MPI_Type_free, (&native_))
  }
  data_type& operator=(const data_type&  that)
  {
    if (this != &that)
    {
      if (managed_ && native_ != MPI_DATATYPE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Type_free, (&native_))
      
      managed_ = true;
      MPI_CHECK_ERROR_CODE(MPI_Type_dup, (that.native_, &native_))
    }
    return *this;
  }
  data_type& operator=(      data_type&& temp) noexcept
  {
    if (this != &temp)
    {
      if (managed_ && native_ != MPI_DATATYPE_NULL)
        MPI_CHECK_ERROR_CODE(MPI_Type_free, (&native_))

      managed_      = temp.managed_;
      native_       = temp.native_;

      temp.managed_ = false;
      temp.native_  = MPI_DATATYPE_NULL;
    }
    return *this;
  }

  [[nodiscard]]
  std::int32_t                size            () const
  {
    std::int32_t result;
    MPI_CHECK_ERROR_CODE(MPI_Type_size, (native_, &result))
    MPI_CHECK_UNDEFINED (MPI_Type_size, result)
    return result;
  }
  [[nodiscard]]
  std::int64_t                size_x          () const
  {
    std::int64_t result;
    MPI_CHECK_ERROR_CODE(MPI_Type_size_x, (native_, &result))
    MPI_CHECK_UNDEFINED (MPI_Type_size_x, result)
    return result;
  }
  [[nodiscard]]
  std::array<std::int64_t, 2> extent          () const
  {
    std::array<std::int64_t, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_extent, (native_, &result[0], &result[1]))
    MPI_CHECK_UNDEFINED (MPI_Type_get_extent, result[0])
    MPI_CHECK_UNDEFINED (MPI_Type_get_extent, result[1])
    return result;
  }
  [[nodiscard]]
  std::array<std::int64_t, 2> extent_x        () const
  {
    std::array<std::int64_t, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_extent_x, (native_, &result[0], &result[1]))
    MPI_CHECK_UNDEFINED (MPI_Type_get_extent_x, result[0])
    MPI_CHECK_UNDEFINED (MPI_Type_get_extent_x, result[1])
    return result;
  }
  [[nodiscard]]
  std::array<std::int64_t, 2> true_extent     () const
  {
    std::array<std::int64_t, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_true_extent, (native_, &result[0], &result[1]))
    MPI_CHECK_UNDEFINED (MPI_Type_get_true_extent, result[0])
    MPI_CHECK_UNDEFINED (MPI_Type_get_true_extent, result[1])
    return result;
  }
  [[nodiscard]]
  std::array<std::int64_t, 2> true_extent_x   () const
  {
    std::array<std::int64_t, 2> result {};
    MPI_CHECK_ERROR_CODE(MPI_Type_get_true_extent_x, (native_, &result[0], &result[1]))
    MPI_CHECK_UNDEFINED (MPI_Type_get_true_extent_x, result[0])
    MPI_CHECK_UNDEFINED (MPI_Type_get_true_extent_x, result[1])
    return result;
  }
  [[nodiscard]]
  data_type_information       information     () const
  {
    data_type_information result;

    std::int32_t integers_size, addresses_size, data_types_size;

    MPI_CHECK_ERROR_CODE(MPI_Type_get_envelope, (
      native_         , 
      &integers_size  , 
      &addresses_size , 
      &data_types_size, 
      reinterpret_cast<std::int32_t*>(&result.combiner)))

    result.integers  .resize (integers_size  );
    result.addresses .resize (addresses_size );
    result.data_types.reserve(data_types_size);

    std::vector<MPI_Datatype> raw_data_types(data_types_size);

    MPI_CHECK_ERROR_CODE(MPI_Type_get_contents, (
      native_                 ,
      integers_size           ,
      addresses_size          ,
      data_types_size         ,
      result.integers  .data(),
      result.addresses .data(),
      raw_data_types   .data()))

    for (auto& type : raw_data_types)
      result.data_types.emplace_back(type, true); // Creates managed data types.

    return result;
  }

  [[nodiscard]]
  std::string                 name            () const
  {
    std::string  result(MPI_MAX_OBJECT_NAME, '\n');
    std::int32_t count (0);
    MPI_CHECK_ERROR_CODE(MPI_Type_get_name, (native_, result.data(), &count))
    result.resize(count);
    return result;
  }
  void                        set_name        (const std::string& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_set_name, (native_, value.data()))
  }

  template <typename type>
  std::optional<type>         attribute       (const data_type_key_value& key) const
  {
    type         result;
    std::int32_t exists;
    MPI_CHECK_ERROR_CODE(MPI_Type_get_attr   , (native_, key.native(), static_cast<void*>(&result), &exists))
    return static_cast<bool>(exists) ? result : std::optional<type>(std::nullopt);
  }
  template <typename type>
  void                        set_attribute   (const data_type_key_value& key, const type& value) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_set_attr   , (native_, key.native(), static_cast<void*>(&value)))
  }
  void                        remove_attribute(const data_type_key_value& key) const
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_delete_attr, (native_, key.native()))
  }

  void                        commit          ()
  {
    MPI_CHECK_ERROR_CODE(MPI_Type_commit, (&native_))
  }

  [[nodiscard]]
  bool                        managed         () const
  {
    return managed_;
  }
  [[nodiscard]]
  MPI_Datatype                native          () const
  {
    return native_;
  }

protected:
  friend class io::file;

  bool         managed_ = false;
  MPI_Datatype native_  = MPI_DATATYPE_NULL;
};
}