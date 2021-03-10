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
        std::cout << " ------------------------------------------------------------------------------- \n"
                  << " ---- Reducing communication overhead via qubit reordering: 1 qubit gate test -- \n"
                  << " ------------------------------------------------------------------------------- \n";
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

    if (num_qubits < 22)
    {
        if (myrank == 0)
            fprintf(stderr, "simulate circuits of at least 22 qubits\n");
        exit(1);
    }

    // Variables for the timing.
    struct timeval time;
    double start, end;
    double duration_trivial_order;
    double duration_optmized_order;

    // We consider a simple circuit, with Hadamard, X, Y, Z acting on the last 10 qubits.
    QubitRegister<ComplexDP> psi_trivial_order(num_qubits, "base", 0);
    psi_trivial_order.ApplyPauliZ(num_qubits - 1); // dummy operation to avoid the first MPI communication during timing
    // Simulate circuit.
    qhipster::mpi::StateBarrier();
    gettimeofday(&time, (struct timezone *)0);
    start = time.tv_sec + time.tv_usec * 1.0e-6;
    for (int q = num_qubits - 10; q < num_qubits; ++q)
    {
        psi_trivial_order.ApplyHadamard(q);
        psi_trivial_order.ApplyPauliX(q);
        psi_trivial_order.ApplyPauliY(q);
        psi_trivial_order.ApplyPauliZ(q);
    }
    qhipster::mpi::StateBarrier();
    gettimeofday(&time, (struct timezone *)0);
    end = time.tv_sec + time.tv_usec * 1.0e-6;
    if (myrank == 0)
        std::cout << "trivial qubit order --> Simulation time = " << end - start << "\n";

    /*--------------------------------------------------------------------------*/
    QubitRegister<ComplexDP> psi_optmized_order(num_qubits, "base", 0);
    psi_optmized_order.ApplyPauliZ(0); // dummy operation to avoid the first MPI communication during timing
    // construct DAG
    DAGCircuit G(num_qubits);
    for (unsigned q = num_qubits - 10; q < num_qubits; ++q)
    {
        G.AddVertex(GateType::Hadamard, q);
        G.AddVertex(GateType::PauliX, q);
        G.AddVertex(GateType::PauliY, q);
        G.AddVertex(GateType::PauliZ, q);
    }
    qhipster::mpi::StateBarrier();
    gettimeofday(&time, (struct timezone *)0);
    start = time.tv_sec + time.tv_usec * 1.0e-6;
    scheduler(G, psi_optmized_order);
    qhipster::mpi::StateBarrier();
    gettimeofday(&time, (struct timezone *)0);
    end = time.tv_sec + time.tv_usec * 1.0e-6;
    if (myrank == 0)
        std::cout << "optmized qubit order --> Simulation time = " << end - start << "\n";

    // Verify that the final state is the same.
    psi_optmized_order.PermuteQubits(psi_trivial_order.qubit_permutation->map, "direct");
    double overlap_sq = std::norm(psi_trivial_order.ComputeOverlap(psi_optmized_order));
    if (myrank == 0)
        std::cout << "Squared overlap of states at the end of the two simulations = " << overlap_sq << "\n\n";
}
