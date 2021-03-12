// #include "../include/gate_schedule.hpp"
#include "../include/qureg.hpp"
#include <iostream>
#include <vector>
#include <sys/time.h>

/// @file gate_schedule.cpp
/// @brief Implement scheduling algorithm to reduce communication overhead.

template <class Type>
void QubitRegister<Type>::ApplyGate(DAGVertex &v)
{
    switch (v.type)
    {
    case GateType::Hadamard:
        assert(v.qubits.size() == 1);
        // std::cout << "Apply Hadamard gate...\nvertex: " << v.id << ", quibit: " << v.qubits[0] << "\n";
        ApplyHadamard(v.qubits[0]);
        break;
    case GateType::PauliX:
        assert(v.qubits.size() == 1);
        // std::cout << "Apply PauliX gate...\nvertex: " << v.id << ", quibit: " << v.qubits[0] << "\n";
        ApplyPauliX(v.qubits[0]);
        break;
    case GateType::PauliY:
        assert(v.qubits.size() == 1);
        // std::cout << "Apply PauliY gate...\nvertex: " << v.id << ", quibit: " << v.qubits[0] << "\n";
        ApplyPauliY(v.qubits[0]);
        break;
    case GateType::PauliZ:
        assert(v.qubits.size() == 1);
        // std::cout << "Apply PauliZ gate...\nvertex: " << v.id << ", quibit: " << v.qubits[0] << "\n";
        ApplyPauliZ(v.qubits[0]);
        break;
    default: // GateType not exist
        assert(false);
        break;
    }
}

void identity_permutation(std::vector<std::size_t> &sigma)
{
    for (std::size_t i = 0; i < sigma.size(); i++)
    {
        sigma[i] = i;
    }
}

bool is_local_qubit(std::vector<std::size_t> &sigma, unsigned qubit, std::size_t m)
{
    return sigma[qubit] < m;
}

// how many local qubits does the gate acts on
int get_local_qubits_num(std::vector<std::size_t> &sigma, std::vector<unsigned> &qubits, std::size_t m)
{
    int count = 0;
    for (auto qubit : qubits)
    {
        count += (sigma[qubit] < m);
    }
    return count;
}

void swap(std::size_t &a, std::size_t &b)
{
    std::size_t temp = a;
    a = b;
    b = temp;
}

void print_permutation(std::vector<std::size_t> &sigma)
{
    std::cout << "Permutation: \n";
    for (int i = 0; i < sigma.size(); i++)
    {
        std::cout << i << " ";
    }
    std::cout << "\n";
    for (int i = 0; i < sigma.size(); i++)
    {
        std::cout << sigma[i] << " ";
    }
    std::cout << "\n";
}

void scheduler(DAGCircuit &G, QubitRegister<> &psi)
{
    struct timeval time1, time2;
    double duration_optmized_order = 0;
    // num of local qubits.
    std::size_t m = G.getQubitNum() - qhipster::ilog2(qhipster::mpi::Environment::GetStateSize());
    int myrank = qhipster::mpi::Environment::GetStateRank();
    if (myrank == 0)
        std::cout << m << " local qubits, " << G.getQubitNum() - m << " global qubits. \n";
    std::vector<std::size_t> sigma(G.getQubitNum());
    identity_permutation(sigma);

    while (G.getVertexNum() > 0)
    {
        for (auto &v : G.vertices)
        {
            if (!v.is_removed && G.getInedgeNum(v) == 0 /* v has no incoming edges */ &&
                get_local_qubits_num(sigma, v.qubits, m) == v.qubits.size() /* v acts on local qubits */)
            {
                qhipster::mpi::StateBarrier();
                gettimeofday(&time1, (struct timezone *)0);

                psi.ApplyGate(v);

                qhipster::mpi::StateBarrier();
                gettimeofday(&time2, (struct timezone *)0);
                duration_optmized_order += time2.tv_sec + time2.tv_usec * 1.0e-6 - time1.tv_sec - time1.tv_usec * 1.0e-6;

                G.RemoveVertex(v);
            }
        }

        int _a = 0;
        int da = 0;
        std::vector<std::size_t> _sigma(G.getQubitNum());
        std::vector<std::size_t> dsigma(G.getQubitNum());
        identity_permutation(_sigma);

        // try all m(n-m) possible permutations
        for (int l = 0; l < G.getQubitNum(); l++)
        {
            if (is_local_qubit(sigma, l, m))
            {
                for (int g = G.getQubitNum() - 1; g >= 0; g--)
                {
                    if (!is_local_qubit(sigma, g, m))
                    {
                        // std::cout << l << " " << g << "\n";
                        // σ' = σ◦(lg)
                        for (int i = 0; i < G.getQubitNum(); i++)
                        {
                            if (sigma[i] == l)
                            {
                                dsigma[i] = g;
                            }
                            else if (sigma[i] == g)
                            {
                                dsigma[i] = l;
                            }
                            else
                            {
                                dsigma[i] = sigma[i];
                            }
                        }
                        // print_permutation(dsigma);
                        da = 0;
                        for (auto &v : G.vertices)
                        {
                            if (!v.is_removed && get_local_qubits_num(dsigma, v.qubits, m) == v.qubits.size())
                            {
                                da++;
                            }
                        }
                        // std::cout << "da: " << da << "\n";
                        if (da > _a)
                        {
                            _sigma = dsigma;
                            _a = da;
                        }
                    }
                }
            }
        }
        if (_a > 0)
        {
            sigma = _sigma;
            // if (myrank == 0)
            //     print_permutation(sigma);
            // qhipster::mpi::StateBarrier();
            // gettimeofday(&time1, (struct timezone *)0);

            psi.PermuteQubits(sigma, "direct");

            // qhipster::mpi::StateBarrier();
            // gettimeofday(&time2, (struct timezone *)0);
            // duration_optmized_order += time2.tv_sec + time2.tv_usec * 1.0e-6 - time1.tv_sec - time1.tv_usec * 1.0e-6;
        }
        else
        {
            // it seems we can not remove communication from any gate by permutating qubits,
            // so we just apply gate directly.
            for (auto &v : G.vertices)
            {
                if (!v.is_removed && G.getInedgeNum(v) == 0 /* v has no incoming edges */)
                {
                    qhipster::mpi::StateBarrier();
                    gettimeofday(&time1, (struct timezone *)0);

                    psi.ApplyGate(v);

                    qhipster::mpi::StateBarrier();
                    gettimeofday(&time2, (struct timezone *)0);
                    duration_optmized_order += time2.tv_sec + time2.tv_usec * 1.0e-6 - time1.tv_sec - time1.tv_usec * 1.0e-6;
                    G.RemoveVertex(v);
                }
            }
        }
    }
    // identity_permutation(sigma);
    // for (std::size_t q = 3; q < G.getQubitNum() - 3; q++)
    // {
    //     // create inverse map.
    //     sigma[G.getQubitNum() - q - 1] = q;
    // }
    // psi.PermuteQubits(sigma, "direct");
    // qhipster::mpi::StateBarrier();
    // gettimeofday(&time1, (struct timezone *)0);
    // for (int q = G.getQubitNum() - 3; q < G.getQubitNum(); ++q)
    // {
    //     psi.ApplyHadamard(q);
    //     psi.ApplyPauliX(q);
    //     psi.ApplyPauliY(q);
    //     psi.ApplyPauliZ(q);
    // }
    // qhipster::mpi::StateBarrier();
    // gettimeofday(&time2, (struct timezone *)0);
    // duration_optmized_order += time2.tv_sec + time2.tv_usec * 1.0e-6 - time1.tv_sec - time1.tv_usec * 1.0e-6;
    if (myrank == 0)
        print_permutation(sigma);
    if (myrank == 0)
        std::cout << "optmized qubit order --> Simulation time = " << duration_optmized_order << "\n";
}