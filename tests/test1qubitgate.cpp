#include <sys/time.h>

#include "../include/qureg.hpp"
int main(int argc, char **argv)
{
    DAGCircuit G;
    int num_qubits = 24;
    for (unsigned q = num_qubits - 1; q < num_qubits; ++q)
    {
        G.AddVertex(GateType::Hadamard, q);
        G.AddVertex(GateType::PauliX, q);
        G.AddVertex(GateType::PauliY, q);
        G.AddVertex(GateType::PauliZ, q);
    }
    G.printGraph();
    for (auto &v : G.vertices)
    {
        // std::cout << G.getVertexNum() << "\n";
        // std::cout << G.getInedgeNum(v) << "\n";
        G.RemoveVertex(v);
        G.printGraph();
    }
}
