/// @file gate_schedule.hpp

#ifndef GATE_SCHEDULE_HPP
#define GATE_SCHEDULE_HPP
#include "dag.hpp"

void identity_permutation(std::vector<std::size_t> &sigma);
bool is_local_qubit(std::vector<std::size_t> &sigma, unsigned qubit, std::size_t m);
int get_local_qubits_num(std::vector<std::size_t> &sigma, std::vector<unsigned> &qubits, std::size_t m);
void scheduler(DAGCircuit &G, QubitRegister<> &psi);

#endif