#pragma once
#include <vector>
// N : vertex attribute
// D : edge attribute
template <class N, class D = float>
class Graph {
public:
    Graph(size_t numVertices) : m_vertices(numVertices), m_edges(numVertices), m_edgeAttributes(numVertices) {}
    virtual void addEdge(size_t fromIndex, size_t toIndex) = 0;
    virtual void addEdge(size_t fromIndex, size_t toIndex, const D& attribute) = 0;
    virtual void addUndirectedEdge(size_t indexA, size_t indexB);
    virtual void addUndirectedEdge(size_t indexA, size_t indexB, const D& attribute);
    virtual std::vector<size_t>& getAdjacent(size_t index) = 0;
    virtual std::vector<D>& getEdgeAttributes(size_t index) = 0;
    virtual D& getEdgeAttribute(size_t fromIndex, size_t toIndex) = 0;
    virtual std::vector<N>& getVertexAttributes() = 0;
    virtual N& getVertexAttribute(size_t index) = 0;
    virtual size_t getNumVertices() const;
    virtual ~Graph() = default;

protected:
    std::vector<N> m_vertices;
    std::vector<std::vector<size_t>> m_edges;
    std::vector<std::vector<D>> m_edgeAttributes;
};



template <class N, class D>
void Graph<N, D>::addUndirectedEdge(size_t indexA, size_t indexB) {
    addEdge(indexA, indexB);
    addEdge(indexB, indexA);
}

template <class N, class D>
void Graph<N, D>::addUndirectedEdge(size_t indexA, size_t indexB, const D& attribute) {
    addEdge(indexA, indexB, attribute);
    addEdge(indexB, indexA, attribute);
}

template <class N, class D>
size_t Graph<N, D>::getNumVertices() const {
    return m_vertices.size();
}