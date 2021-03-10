/// @file gate_schedule.hpp

#ifndef GATE_SCHEDULE_HPP
#define GATE_SCHEDULE_HPP
#include "dag.hpp"
void identity_permutation(std::vector<unsigned> &sigma);
bool is_local_qubit(std::vector<unsigned> &sigma, unsigned qubit, unsigned m);
int get_local_qubits_num(std::vector<unsigned> &sigma, std::vector<unsigned> &qubits, unsigned m);
void scheduler(DAGCircuit &G);
#endif