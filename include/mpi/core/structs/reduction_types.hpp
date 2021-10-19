#pragma once

#include <cstdint>

namespace mpi
{
struct short_int
{
  std::int16_t first ;
  std::int32_t second;
};
struct two_int
{
  std::int32_t first ;
  std::int32_t second;
};
struct long_int
{
  long         first ;
  std::int32_t second;
};

struct float_int
{
  float        first ;
  std::int32_t second;
};
struct double_int
{
  double       first ;
  std::int32_t second;
};
struct long_double_int
{
  long double  first ;
  std::int32_t second;
};
}