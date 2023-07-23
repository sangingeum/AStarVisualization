#pragma once
#include "Graph.hpp"
#include <stdexcept>
template <class N, class D = float>
class AdjacencyListGraph : public Graph<N, D>
{
public:
    AdjacencyListGraph<N, D>(size_t numVertices) : Graph<N, D>(numVertices) {}
    void addEdge(size_t fromIndex, size_t toIndex);
    void addEdge(size_t fromIndex, size_t toIndex, const D& attribute);
    std::vector<size_t>& getAdjacent(size_t index);
    std::vector<D>& getEdgeAttributes(size_t index);
    D& getEdgeAttribute(size_t fromIndex, size_t toIndex);
    std::vector<N>& getVertexAttributes();
    N& getVertexAttribute(size_t index);
};

template <class N, class D>
void AdjacencyListGraph<N, D>::addEdge(size_t fromIndex, size_t toIndex) {
    this->m_edges[fromIndex].push_back(toIndex);
}

template <class N, class D>
void AdjacencyListGraph<N, D>::addEdge(size_t fromIndex, size_t toIndex, const D& attribute) {
    addEdge(fromIndex, toIndex);
    this->m_edgeAttributes[fromIndex].push_back(attribute);
}

template <class N, class D>
std::vector<size_t>& AdjacencyListGraph<N, D>::getAdjacent(size_t index) {
    return this->m_edges[index];
}

template <class N, class D>
std::vector<D>& AdjacencyListGraph<N, D>::getEdgeAttributes(size_t index) {
    return this->m_edgeAttributes[index];
}
// Time Complexity : O(N)
template <class N, class D>
D& AdjacencyListGraph<N, D>::getEdgeAttribute(size_t fromIndex, size_t toIndex) {
    auto& vertices = getAdjacent(fromIndex);
    size_t size = vertices.size();
    for (size_t i = 0; i < size; ++i) {
        if (vertices[i] == toIndex) {
            return getEdgeAttributes(fromIndex)[i];
        }
    }
    throw std::out_of_range("out_of_range in getEdge");
}

template <class N, class D>
std::vector<N>& AdjacencyListGraph<N, D>::getVertexAttributes() {
    return this->m_vertices;
}

template <class N, class D>
N& AdjacencyListGraph<N, D>::getVertexAttribute(size_t index) {
    return this->m_vertices[index];
}