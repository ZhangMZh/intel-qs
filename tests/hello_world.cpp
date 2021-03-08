//------------------------------------------------------------------------------
// Copyright (C) 2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//------------------------------------------------------------------------------

#include <sys/time.h>

#include "../include/dag.hpp"

/// @file communication_reduction_via_qubit_reordering.cpp
/// Simple program to illustrate how the communication overhead between MPI
/// processes can be reduced by reordering the qubits.

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  qhipster::mpi::Environment env(argc, argv);
  if (env.IsUsefulRank() == false)
    return 0;
  int myrank = env.GetStateRank();
  if (myrank == 0)
    std::cout << " ---------------------- \n"
              << " ---- Hello world! ---- \n"
              << " ---------------------- \n";

  hello();
}
