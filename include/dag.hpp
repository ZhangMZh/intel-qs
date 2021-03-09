/// @file dag.hpp
/// @brief Declare the @c DAGCircuit class.

#ifndef DAG_HPP
#define DAG_HPP

#include <vector>
#include <set>
#include <map>
#include <algorithm>

enum class GateType
{
    Hadamard,
    PauliX,
    PauliY,
    PauliZ,
};

typedef struct DAGVertex
{
    int id;
    GateType type;
    std::vector<unsigned> qubits;
    bool is_removed;
} DAGVertex;

void hello();

class DAGCircuit
{
private:
    int v_num = 0;

public:
    std::vector<DAGVertex> vertices;
    std::map<int, std::set<int>> v2inedges;
    std::map<int, std::set<int>> v2outedges;
    // Constructors and destructors.
    DAGCircuit() = default;
    ~DAGCircuit() = default;

    template <typename... T>
    void AddVertex(GateType type, T... args)
    {
        std::vector<unsigned> qubits{std::forward<T>(args)...};
        return AddVertex(type, qubits);
    };
    void AddVertex(GateType type, std::vector<unsigned> qubits);

    // remove a vertex and all its outcoming edges
    void RemoveVertex(DAGVertex &v);

    // num of vertices in current graph
    int getVertexNum();
    // calculate how many incoming edges does vertex v have currently
    int getInedgeNum(DAGVertex &v);

    // print graph for debug
    void printGraph();
};
#endif