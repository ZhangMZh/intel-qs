#include <mpi.h>

#include "../include/qureg.hpp"
#include "../include/gate_schedule.hpp"

int main(int argc, char **argv)
{
    qhipster::mpi::Environment env(argc, argv);
    if (env.IsUsefulRank() == false)
        return 0;
    int myrank = env.GetStateRank();
    if (myrank == 0)
        std::cout << " --------------------------------------------------------------------------------- \n"
                  << " ---- Reducing communication overhead via qubit reordering: 1 qubit gate test ---- \n"
                  << " --------------------------------------------------------------------------------- \n";
    int num_qubits = 22;
    if (argc != 2)
    {
        if (myrank == 0)
            fprintf(stderr, "usage: %s <num_qubits>\n", argv[0]);
        exit(1);
    }
    else
    {
        num_qubits = atoi(argv[1]);
    }

    // Variables for the timing.
    double start, end;

    // We consider a simple circuit, with Hadamard, X, Y, Z acting on the last 10 qubits.
    QubitRegister<ComplexDP> psi_trivial_order(num_qubits, "base", 0);
    // Simulate circuit.
    qhipster::mpi::StateBarrier();
    psi_trivial_order.EnableStatistics();
    start = MPI_Wtime();

    // for (int q = num_qubits - 10; q < num_qubits; ++q)
    // {
    //     psi_trivial_order.ApplyCPauliX(num_qubits - q - 1, q);
    //     psi_trivial_order.ApplyCPauliY(num_qubits - q - 1, q);
    //     psi_trivial_order.ApplyCPauliZ(num_qubits - q - 1, q);
    // }
    end = MPI_Wtime();
    psi_trivial_order.GetStatistics();
    psi_trivial_order.DisableStatistics();

    if (myrank == 0)
        std::cout << "trivial qubit order --> Simulation time = " << end - start << "\n";

    /*--------------------------------------------------------------------------*/
    QubitRegister<ComplexDP> psi_optmized_order(num_qubits, "base", 0);
    // construct DAG
    DAGCircuit G(num_qubits);
    for (unsigned q = num_qubits - 10; q < num_qubits; ++q)
    {
        G.AddVertex(GateType::CPauliX, num_qubits - q - 1, q);
        G.AddVertex(GateType::CPauliY, num_qubits - q - 1, q);
        G.AddVertex(GateType::CPauliZ, num_qubits - q - 1, q);
    }

    scheduler(G, psi_optmized_order);

    // Verify that the final state is the same.
    psi_optmized_order.PermuteQubits(psi_trivial_order.qubit_permutation->map, "direct");
    double overlap_sq = std::norm(psi_trivial_order.ComputeOverlap(psi_optmized_order));
    if (myrank == 0)
        std::cout << "Squared overlap of states at the end of the two simulations = " << overlap_sq << "\n\n";
}
