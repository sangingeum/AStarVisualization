#pragma once
#include "AdjacencyListGraph.hpp"
#include "FibonacciHeap.hpp"
#include "Entity.hpp"
#include <limits>
#include <functional>

/*Example Vertex
struct Vertex {
	std::pair<float, float> pos;
	float gScore, fScore;
	size_t parent;
};
*/

template <class Vertex>
class AStar
{
public:
	static std::vector<std::pair<size_t, float>> shortestPath(AdjacencyListGraph<Vertex>& graph, size_t from, size_t to,
		std::function<float(const std::pair<float, float>&, const std::pair<float, float>&)> heuristic = NoHeuristic);
private:
	static void initialize(AdjacencyListGraph<Vertex>& graph, size_t from, size_t to);
	static float NoHeuristic(const std::pair<float, float>& posA, const std::pair<float, float>& posB) {
		return 0;
	}
	static void setColor(sf::VertexArray& array, sf::Color color) {
		size_t vertexSize = array.getVertexCount();
		for (size_t i = 0; i < vertexSize; ++i)
			array[i].color = color;
	}
};

// Find a path from a vertex to another vertex in a graph using the A* algorithm
// A heuristic function should be passed as an argument to guide the search.
// If not given, it runs the Dijksta's algorithm instead.
template <class Vertex>
std::vector<std::pair<size_t, float>> AStar<Vertex>::shortestPath(AdjacencyListGraph<Vertex>& graph, size_t from, size_t to,
	std::function<float(const std::pair<float, float>&, const std::pair<float, float>&)> heuristic) {
	size_t numVetices = graph.getNumVertices();
	// Initialize the graph's gScroes, fScores, and parents
	initialize(graph, from, to);

	// Create a minQ that stores vetices
	FibonacciHeap<size_t> minQ;
	// Make a visited vector to prevent redundant calulations
	std::vector<bool> visited(numVetices, false);
	// Make a handle vector for the decreaseKey operation
	std::vector<FibonacciHeap<size_t>::Handle> handles(numVetices);


	handles[from] = minQ.push(graph.getVertexAttribute(from).fScore, from);

	auto& goalAtt = graph.getVertexAttribute(to);
	auto& startAtt = graph.getVertexAttribute(from);
	auto startColor = startAtt.block->getComponent<CShape>()->vertexArray[0].color;
	auto endColor = goalAtt.block->getComponent<CShape>()->vertexArray[0].color;

	while (!minQ.empty()) {
		auto cur = minQ.top(); minQ.pop();
		if (cur == to) {
			// Found the target. Stop searching
			break;
		}

		if (!visited[cur]) {
			visited[cur] = true;
			auto& adjs = graph.getAdjacent(cur);
			auto& edgeAtts = graph.getEdgeAttributes(cur);
			auto& curAtt = graph.getVertexAttribute(cur);
			setColor(curAtt.block->getComponent<CShape>()->vertexArray, sf::Color::Green);
			size_t adjSize = adjs.size();
			for (size_t i = 0; i < adjSize; ++i) {
				size_t neighbor = adjs[i];
				auto& neighborAtt = graph.getVertexAttribute(neighbor);
				if (neighborAtt.block->getComponent<CBlock>()->isObstacle)
					continue;
				float tentativeGScore = curAtt.gScore + edgeAtts[i];
				// If the path through the current vertex is better, update the neighbor
				if (!visited[neighbor] && (neighborAtt.gScore > tentativeGScore)) {
					neighborAtt.parent = cur;
					neighborAtt.gScore = tentativeGScore;
					neighborAtt.fScore = tentativeGScore + heuristic(neighborAtt.pos, goalAtt.pos);

					// Min priority queue selects the next vertex based on the fScore which is the sum of the gScore and the hScore.
					
					// Push the neighbor to the minQ with neighbor's fScore as a key if the neighbor node hasn't been pushed before
					if (handles[neighbor].isNull()) {
						handles[neighbor] = minQ.push(neighborAtt.fScore, neighbor);
						setColor(neighborAtt.block->getComponent<CShape>()->vertexArray, sf::Color::White);
					}
					// Decrease the key otherwise
					else
						minQ.decreaseKey(handles[neighbor], neighborAtt.fScore);
					
				}
			}
		}
	}
	

	// Construct the path using the vertex attributes calculated above
	std::vector<std::pair<size_t, float>> path;
	size_t cur = to;
	while (cur < numVetices) {
		auto& att = graph.getVertexAttribute(cur);
		path.push_back({ cur, att.gScore });
		cur = att.parent;
	}
	
	// Color the path
	for (auto& step : path) {
		setColor(graph.getVertexAttribute(step.first).block->getComponent<CShape>()->vertexArray, sf::Color(255, 0, 255, 255));
	}
	// Reverse the order
	path = std::vector<std::pair<size_t, float>>(path.rbegin(), path.rend());

	// Reset Start and End color
	setColor(startAtt.block->getComponent<CShape>()->vertexArray, startColor);
	setColor(goalAtt.block->getComponent<CShape>()->vertexArray, endColor);

	
	return path;
}

template <class Vertex>
void AStar<Vertex>::initialize(AdjacencyListGraph<Vertex>& graph, size_t from, size_t to) {
	// Set initial values
	auto& vertexAtts = graph.getVertexAttributes();
	for (auto& att : vertexAtts) {
		att.gScore = std::numeric_limits<float>::infinity();
		att.fScore = std::numeric_limits<float>::infinity();
		att.parent = std::numeric_limits<size_t>::max();
	}
	// Set the gScore and the fScore of the source vertex to 0
	vertexAtts[from].gScore = 0;
	vertexAtts[from].fScore = 0;
}

/*
template <class Vertex>
float AStar<Vertex>::heuristic(const std::pair<float, float>& posA, const std::pair<float, float>& posB) {
	// Calculate the squared euclidian distance -> quite greedy
	return powf(posA.first - posB.first, 2) + powf(posA.second - posB.second, 2);
}
*/
