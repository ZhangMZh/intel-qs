// #include "../include/gate_schedule.hpp"
#include "../include/qureg.hpp"
#include <iostream>
#include <vector>
/// @file gate_schedule.cpp
/// @brief Implement scheduling algorithm to reduce communication overhead.

template <class Type>
void QubitRegister<Type>::ApplyGate(DAGVertex v)
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

void scheduler(DAGCircuit &G)
{
    while (G.getVertexNum() > 0)
    {
        for (auto &v : G.vertices)
        {
            if (!v.is_removed && G.getInedgeNum(v) == 0)
            {
            }
        }
        //     for (int v = 0; (v < G.max_V) && (G.ingraph(v)); v++)
        //     {
        //         if ((G.inedges(v) == 0) && (sigma[v] > 10))
        //         {
        //             //AddGate(v)
        //             G.remove(v);
        //         }
        //     }

        //     int _a = 0;
        //     int da = 0;
        //     vector<int> _sigma(G.max_V, 0);
        //     vector<int> dsigma(G.max_V, 0);
        //     for (int i = 0; i < G.max_V; i++)
        //     {
        //         _sigma[i] = i;
        //         dsigma[i] = i;
        //     }

        //     for (int l = 0; (l < G.max_V) && (sigma[l] < m); l++)
        //     {
        //         for (int g = 0; (g < G.max_V) && (sigma[g] >= m); g++)
        //         {
        //             // ?
        //             da = 0;
        //             for (int j = 0; (j < G.max_V) && (G.ingraph(j)); j++)
        //             {
        //                 if (dsigma[j] < m)
        //                     da++;
        //             }
        //             if (da > _a)
        //             {
        //                 for (int j = 0; j < G.max_V; j++)
        //                 {
        //                     _sigma[j] = dsigma[j];
        //                 }
        //                 _a = da;
        //             }
        //         }
        //     }
        //     if (_a > 0)
        //     {
        //         for (int j = 0; j < G.max_V; j++)
        //         {
        //             sigma[j] = _sigma[j];
        //         }
        //         // add permutation
        //     }
        //     else
        //     {
        //         for (int v = 0; (v < G.max_V) && (G.ingraph(v)) && (G.inedges(v) == 0); v++)
        //         {
        //             // add v
        //             G.remove(v);
        //         }
        //     }
    }
}