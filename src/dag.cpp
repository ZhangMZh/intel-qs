#include "../include/dag.hpp"
#include <iostream>
#include <assert.h>

/// @file dag.cpp
/// @brief Define the @c DAGCircuit method.

using namespace std;

void hello()
{
    cout << "Hello world!\n"
         << endl;
}

void DAGCircuit::AddVertex(GateType type, std::vector<unsigned> qubits)
{
    DAGVertex v = {(int)vertices.size(), type, qubits, false};
    // // Construct dependencies(add edges)
    std::set<int> inedges;
    std::set<int> outedges;
    for (auto qubit : qubits)
    {
        for (int i = vertices.size() - 1; i >= 0; i--)
        {
            auto &u = vertices[i];
            if (std::find(u.qubits.begin(), u.qubits.end(), qubit) != u.qubits.end())
            {
                inedges.insert(u.id);
                v2outedges.at(u.id).insert(v.id);
                break;
            }
        }
    }
    v2inedges.insert({v.id, inedges});
    v2outedges.insert({v.id, outedges});
    vertices.push_back(v);
    v_num++;
}

void DAGCircuit::RemoveVertex(DAGVertex &v)
{
    // Remove vertex
    v.is_removed = true;

    // Remove edge
    std::set<int> &outedges = v2outedges.at(v.id);
    for (auto id : outedges)
    {
        v2inedges.at(id).erase(v.id);
    }
    v2inedges.erase(v.id);
    v2outedges.erase(v.id);
    v_num--;
}

int DAGCircuit::getInedgeNum(DAGVertex &v)
{
    assert(!v.is_removed);
    auto &inedges = v2inedges.at(v.id);
    return inedges.size();
}

int DAGCircuit::getVertexNum()
{
    return v_num;
}

void DAGCircuit::printGraph()
{
    std::cout << " --------------------------------------------- \n";
    for (auto &v : vertices)
    {
        if (!v.is_removed)
        {
            std::cout << " vertex id: " << v.id << "\n";
            std::cout << " incoming edges: ";
            for (auto id : v2inedges.at(v.id))
            {
                std::cout << id << " ";
            }
            std::cout << "\n outcoming edges: ";
            for (auto id : v2outedges.at(v.id))
            {
                std::cout << id << " ";
            }
            std::cout << "\n\n";
        }
    }
    std::cout << " --------------------------------------------- \n";
}