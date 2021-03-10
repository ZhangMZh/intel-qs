// #include "../include/gate_schedule.hpp"
#include "../include/qureg.hpp"
#include <iostream>
#include <vector>
/// @file gate_schedule.cpp
/// @brief Implement scheduling algorithm to reduce communication overhead.

template <class Type>
void QubitRegister<Type>::ApplyGate(DAGVertex &v)
{
    switch (v.type)
    {
    case GateType::Hadamard:
        assert(v.qubits.size() == 1);
        ApplyHadamard(v.qubits[0]);
        break;
    case GateType::PauliX:
        assert(v.qubits.size() == 1);
        ApplyPauliX(v.qubits[0]);
        break;
    case GateType::PauliY:
        assert(v.qubits.size() == 1);
        ApplyPauliY(v.qubits[0]);
        break;
    case GateType::PauliZ:
        assert(v.qubits.size() == 1);
        ApplyPauliZ(v.qubits[0]);
        break;
    }
}

void identity_permutation(vector<unsigned> &sigma)
{
    for (unsigned i = 0; i < sigma.size(); i++)
    {
        sigma[i] = i;
    }
}

bool is_local_qubit(vector<unsigned> &sigma, unsigned qubit, unsigned m)
{
    return sigma[qubit] < m;
}

// how many local qubits does the gate acts on
int get_local_qubits_num(vector<unsigned> &sigma, std::vector<unsigned> &qubits, unsigned m)
{
    int count = 0;
    for (auto qubit : qubits)
    {
        count += (sigma[qubit] < m);
    }
    return count;
}

void swap(unsigned &a, unsigned &b)
{
    unsigned temp = a;
    a = b;
    b = temp;
}

void scheduler(DAGCircuit &G)
{
    // num of local qubits.
    unsigned m = G.getQubitNum() - qhipster::ilog2(qhipster::mpi::Environment::GetStateSize());

    vector<unsigned> sigma(G.getQubitNum());
    identity_permutation(sigma);

    while (G.getVertexNum() > 0)
    {
        for (auto &v : G.vertices)
        {
            if (!v.is_removed && G.getInedgeNum(v) == 0 /* v has no incoming edges */ &&
                get_local_qubits_num(sigma, v.qubits, m) == v.qubits.size() /* v acts on local qubits */)
            {
                // TODO: apply gate
                G.RemoveVertex(v);
            }
        }

        int _a = 0;
        int da = 0;
        vector<unsigned> _sigma(G.getQubitNum());
        vector<unsigned> dsigma(G.getQubitNum());
        identity_permutation(_sigma);
        identity_permutation(dsigma);

        // try all m(n-m) possible permutations
        for (int l = 0; l < G.getQubitNum(); l++)
        {
            if (is_local_qubit(sigma, l, m))
            {
                for (int g = 0; g < G.getQubitNum(); g++)
                {
                    if (!is_local_qubit(sigma, g, m))
                    {
                        // σ' = σ◦(lg)
                        for (int i = 0; i < G.getQubitNum(); i++)
                        {
                            if (sigma[i] == l)
                            {
                                dsigma[i] = g;
                            }
                            else if (sigma[i] = g)
                            {
                                dsigma[i] = l;
                            }
                            else
                            {
                                dsigma[i] = sigma[i];
                            }
                        }

                        da = 0;
                        for (auto &v : G.vertices)
                        {
                            if (!v.is_removed && get_local_qubits_num(dsigma, v.qubits, m) == v.qubits.size())
                            {
                                da++;
                            }
                        }

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
            // TODO: permutate qubits
        }
        else
        {
            // it seems we can not remove communication from any gate by permutating qubits,
            // so we just apply gate directly.
            for (auto &v : G.vertices)
            {
                if (!v.is_removed && G.getInedgeNum(v) == 0 /* v has no incoming edges */)
                {
                    // TODO: apply gate
                    G.RemoveVertex(v);
                }
            }
        }
    }
}