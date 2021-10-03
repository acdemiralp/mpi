#pragma once

#include <mpi/core/mpi.hpp>

namespace mpi
{
// Message is only a typedef to an int. Reasoning: It has no member functions except MPI_Message_c2f/f2c which are omitted in this wrapper.
using message = MPI_Message;
}