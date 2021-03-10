#include <sys/time.h>

#include "../include/qureg.hpp"
#include "../include/gate_schedule.hpp"

int main(int argc, char **argv)
{
    qhipster::mpi::Environment env(argc, argv);
    if (env.IsUsefulRank() == false)
        return 0;
    int myrank = env.GetStateRank();
    if (myrank == 0)
        std::cout << " ------------------------------------------------------------ \n"
                  << " ---- Reducing communication overhead via qubit reordering -- \n"
                  << " ------------------------------------------------------------ \n";
    int num_qubits = 22;
    DAGCircuit G(num_qubits);
    for (unsigned q = num_qubits - 10; q < num_qubits; ++q)
    {
        G.AddVertex(GateType::Hadamard, q);
        G.AddVertex(GateType::PauliX, q);
        G.AddVertex(GateType::PauliY, q);
        G.AddVertex(GateType::PauliZ, q);
    }
    scheduler(G);
}
