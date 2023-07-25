#pragma once
#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>


template <size_t K, class Data, class T = float>
class KDTree
{
private:
	using Point = typename std::array<T, K>;
	using PointDataPair = typename std::pair<Point, Data>;
	struct Node {
		Point point;
		Data data;
		std::shared_ptr<Node> left;
		std::shared_ptr<Node> right;
		Node(const Point& point_, const Data& data_) : point(point_), data(data_) {}
		Node(const PointDataPair& pair) : point(pair.first), data(pair.second) {}
	};
	std::shared_ptr<Node> m_root;
public:
	KDTree() = default;
	KDTree(std::vector<PointDataPair>& pairs) { m_root = buildTree(pairs, 0, 0, pairs.size()); }
	void insert(const PointDataPair& pair);
	PointDataPair findNearestNeighbor(const Point& target);
	std::vector<PointDataPair> searchRange(const Point& lowerBound, const Point& upperBound);
	void buildTree(std::vector<PointDataPair>& pairs);
	void clear();

private:
	std::shared_ptr<Node> buildTree(std::vector<PointDataPair>& pairs, size_t depth, size_t from, size_t to);
	PointDataPair findNearestNeighbor(std::shared_ptr<Node> node, const Point& target, size_t depth);
	T getSquaredDistance(const Point& p1, const Point& p2);
	void searchRange(std::shared_ptr<Node> node, const Point& lowerBound, const Point& upperBound,
		size_t depth, std::vector<PointDataPair>& results);
};

// Insert a point data pair into the tree
// This method does not guarantee the resulting tree will be balanced
template <size_t K, class Data, class T>
void KDTree<K, Data, T>::insert(const PointDataPair& pair) {
	if (!m_root) {
		m_root.reset(new Node(pair));
	}
	else {
		const auto& point = pair.first;
		size_t curDepth = 0;
		std::shared_ptr<Node> curNode = m_root;
		std::shared_ptr<Node> nextNode = m_root;
		while (true) {
			const T& curValue = curNode->point[curDepth];
			if (curValue < point[curDepth]) {
				nextNode = curNode->right;
				if (!nextNode) {
					curNode->right.reset(new Node(pair));
					break;
				}
			}
			else {
				nextNode = curNode->left;
				if (!nextNode) {
					curNode->left.reset(new Node(pair));
					break;
				}
			}
			curNode = nextNode;
			curDepth = (curDepth + 1) % K;
		}
	}
}

// Find a nearest point data pair to the given point in the tree
template <size_t K, class Data, class T>
typename KDTree<K, Data, T>::PointDataPair KDTree<K, Data, T>::findNearestNeighbor(const Point& target) {
	if (!m_root)
		throw std::runtime_error("Tree is empty");
	return findNearestNeighbor(m_root, target, 0);
}

// Find point data pairs in the given range
template <size_t K, class Data, class T>
std::vector<typename KDTree<K, Data, T>::PointDataPair> KDTree<K, Data, T>::searchRange(
	const Point& lowerBound, const Point& upperBound) {

	std::vector<PointDataPair> results;
	searchRange(m_root, lowerBound, upperBound, 0, results);
	return results;
}

// Clear the tree and Rebuild the tree using the given point data pairs
template <size_t K, class Data, class T>
void KDTree<K, Data, T>::buildTree(std::vector<PointDataPair>& pairs) {
	clear();
	m_root = buildTree(pairs, 0, 0, pairs.size());
}

// Clear the tree
template <size_t K, class Data, class T>
void KDTree<K, Data, T>::clear() {
	m_root.reset();
}

// Build a balanced k-d tree from the point data pair vector
template <size_t K, class Data, class T>
std::shared_ptr<typename KDTree<K, Data, T>::Node> KDTree<K, Data, T>::buildTree(std::vector<PointDataPair>& pairs, size_t depth, size_t from, size_t to) {
	size_t kIndex = depth % K;
	size_t nthIndex = (from + to) / 2;
	auto nth = nthIndex + pairs.begin();
	// Find the median point at depth 'kIndex'
	std::nth_element(pairs.begin() + from, nth, pairs.begin() + to,
		[kIndex](const PointDataPair& a, const PointDataPair& b) {
			return a.first[kIndex] < b.first[kIndex];
		});
	auto curNode = std::make_shared<Node>(*nth);
	// Extend children
	if (nthIndex - from > 0)
		curNode->left = buildTree(pairs, depth + 1, from, nthIndex);
	if (to - nthIndex > 1)
		curNode->right = buildTree(pairs, depth + 1, nthIndex + 1, to);
	return curNode;
}

// private findNearestNeighbor helper
template <size_t K, class Data, class T>
typename KDTree<K, Data, T>::PointDataPair KDTree<K, Data, T>::findNearestNeighbor(
	std::shared_ptr<Node> node, const Point& target, size_t depth) {

	size_t curDepth = depth % K;
	std::shared_ptr<Node> nextBranch;
	std::shared_ptr<Node> otherBranch;
	// Determine the next and other branches 
	// based on the point values at curDepth
	if (target[curDepth] < node->point[curDepth]) {
		nextBranch = node->left;
		otherBranch = node->right;
	}
	else {
		nextBranch = node->right;
		otherBranch = node->left;
	}
	T bestDist;
	PointDataPair bestPair;
	PointDataPair tempPair;

	// Explore the next branch if it is not null
	if (nextBranch) {
		// Compare 'node->point' and 'tempPoint' and choose the better point
		tempPair = findNearestNeighbor(nextBranch, target, depth + 1);
		T distToRoot = getSquaredDistance(target, node->point);
		T distToTemp = getSquaredDistance(target, tempPair.first);
		if (distToRoot > distToTemp) {
			bestDist = distToTemp;
			bestPair = tempPair;
		}
		else {
			bestDist = distToRoot;
			bestPair = { node->point, node->data };
		}
	}
	// Handle the case when 'nextBranch' is null
	else {
		bestPair = { node->point, node->data };
		bestDist = getSquaredDistance(target, node->point);
	}

	// Explore 'otherbranch' if it is not null
	if (otherBranch) {
		T diff = target[curDepth] - node->point[curDepth];
		T distToOtherBranch = diff * diff;
		// Search 'otherBranch' if bestDist >= distToOtherBranc
		if (bestDist >= distToOtherBranch) {
			tempPair = findNearestNeighbor(otherBranch, target, depth + 1);
			if (bestDist > getSquaredDistance(target, tempPair.first))
				bestPair = tempPair;
		}
	}
	return bestPair;
}

// Calculate the distance between two points
template <size_t K, class Data, class T>
T KDTree<K, Data, T>::getSquaredDistance(const Point& p1, const Point& p2) {
	T distance = 0;
	for (size_t i = 0; i < K; ++i) {
		T diff = p1[i] - p2[i];
		distance += diff * diff;
	}
	return distance;
}

// private searchRange helper
template <size_t K, class Data, class T>
void KDTree<K, Data, T>::searchRange(std::shared_ptr<Node> node, const Point& lowerBound, const Point& upperBound,
	size_t depth, std::vector<PointDataPair>& results) {
	if (!node)
		return;
	// Check the current node
	bool inRange{ true };
	for (size_t i = 0; i < K; ++i) {
		if (node->point[i] < lowerBound[i] || node->point[i] > upperBound[i]) {
			inRange = false;
			break;
		}
	}

	// If it is in the range, append it to the vector
	if (inRange)
		results.push_back({ node->point, node->data });

	// Determine which branch to search
	size_t nextDepth = (depth + 1) % K;
	const T& curValue = node->point[depth];
	if (curValue >= lowerBound[depth])
		searchRange(node->left, lowerBound, upperBound, nextDepth, results);
	if (curValue <= upperBound[depth])
		searchRange(node->right, lowerBound, upperBound, nextDepth, results);
}