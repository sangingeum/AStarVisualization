#pragma once
#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

// TODO
// Remove, Min, Max operations

template <size_t K, class T = float>
class KDTree
{
private:
	struct Node {
		std::array<T, K> point;
		std::shared_ptr<Node> left;
		std::shared_ptr<Node> right;
		Node(const std::array<T, K>& point_) : point(point_) {}
	};
	std::shared_ptr<Node> m_root;
public:
	KDTree() = default;
	KDTree(std::vector<std::array<T, K>>& points) { m_root = buildTree(points, 0, 0, points.size()); }
	void insert(const std::array<T, K>& point);
	std::array<T, K> findNearestNeighbor(const std::array<T, K>& target);
	std::vector<std::array<T, K>> searchRange(const std::array<T, K>& lowerBound, const std::array<T, K>& upperBound);
private:
	std::shared_ptr<Node> buildTree(std::vector<std::array<T, K>>& points, size_t depth, size_t from, size_t to);
	std::array<T, K> findNearestNeighbor(std::shared_ptr<Node> root, const std::array<T, K>& target, size_t depth);
	T getSquaredDistance(const std::array<T, K>& p1, const std::array<T, K>& p2);
	void searchRange(std::shared_ptr<Node> node, const std::array<T, K>& lowerBound, const std::array<T, K>& upperBound,
		size_t depth, std::vector<std::array<T, K>>& results);
};

// Insert a point into the tree
// This method does not guarantee the resulting tree will be balanced
template <size_t K, class T>
void KDTree<K, T>::insert(const std::array<T, K>& point) {
	if (!m_root) {
		m_root.reset(new Node(point));
	}
	else {
		size_t curDepth = 0;
		std::shared_ptr<Node> curNode = m_root;
		std::shared_ptr<Node> nextNode = m_root;
		while (true) {
			const T& curValue = curNode->point[curDepth];
			if (curValue < point[curDepth]) {
				nextNode = curNode->right;
				if (!nextNode) {
					curNode->right.reset(new Node(point));
					break;
				}
			}
			else {
				nextNode = curNode->left;
				if (!nextNode) {
					curNode->left.reset(new Node(point));
					break;
				}
			}
			curNode = nextNode;
			curDepth = (curDepth + 1) % K;
		}
	}
}

// Find a nearest point to the given target in the tree
template <size_t K, class T>
std::array<T, K> KDTree<K, T>::findNearestNeighbor(const std::array<T, K>& target) {
	if (!m_root)
		throw std::runtime_error("Tree is empty");
	return findNearestNeighbor(m_root, target, 0);
}

// Find points in the given range
template <size_t K, class T>
std::vector<std::array<T, K>> KDTree<K, T>::searchRange(const std::array<T, K>& lowerBound, const std::array<T, K>& upperBound) {
	std::vector<std::array<T, K>> results;
	searchRange(m_root, lowerBound, upperBound, 0, results);
	return results;
}



// Build a balanced k-d tree from the points vector
template <size_t K, class T>
std::shared_ptr<typename KDTree<K, T>::Node> KDTree<K, T>::buildTree(std::vector<std::array<T, K>>& points, size_t depth, size_t from, size_t to) {
	size_t kIndex = depth % K;
	size_t nthIndex = (from + to) / 2;
	auto nth = nthIndex + points.begin();
	// Find the median point at depth 'kIndex'
	std::nth_element(points.begin() + from, nth, points.begin() + to,
		[kIndex](const std::array<T, K>& a, const std::array<T, K>& b) {
			return a[kIndex] < b[kIndex];
		});
	auto curNode = std::make_shared<Node>(*nth);
	// Extend children
	if (nthIndex - from > 0)
		curNode->left = buildTree(points, depth + 1, from, nthIndex);
	if (to - nthIndex > 1)
		curNode->right = buildTree(points, depth + 1, nthIndex + 1, to);
	return curNode;
}

// private findNearestNeighbor helper
template <size_t K, class T>
std::array<T, K> KDTree<K, T>::findNearestNeighbor(std::shared_ptr<Node> root, const std::array<T, K>& target, size_t depth) {
	size_t curDepth = depth % K;
	std::shared_ptr<Node> nextBranch;
	std::shared_ptr<Node> otherBranch;
	// Determine the next and other branches 
	// based on the point values at curDepth
	if (target[curDepth] < root->point[curDepth]) {
		nextBranch = root->left;
		otherBranch = root->right;
	}
	else {
		nextBranch = root->right;
		otherBranch = root->left;
	}
	T bestDist;
	std::array<T, K> bestPoint;
	std::array<T, K> tempPoint;

	// Explore the next branch if it is not null
	if (nextBranch) {
		// Compare 'root->point' and 'tempPoint' and choose the better point
		tempPoint = findNearestNeighbor(nextBranch, target, depth + 1);
		T distToRoot = getSquaredDistance(target, root->point);
		T distToTemp = getSquaredDistance(target, tempPoint);
		if (distToRoot > distToTemp) {
			bestDist = distToTemp;
			bestPoint = tempPoint;
		}
		else {
			bestDist = distToRoot;
			bestPoint = root->point;
		}
	}
	// Handle the case when 'nextBranch' is null
	else {
		bestPoint = root->point;
		bestDist = getSquaredDistance(target, root->point);
	}

	// Explore 'otherbranch' if it is not null
	if (otherBranch) {
		T diff = target[curDepth] - root->point[curDepth];
		T distToOtherBranch = diff * diff;
		// Search 'otherBranch' if bestDist >= distToOtherBranc
		if (bestDist >= distToOtherBranch) {
			tempPoint = findNearestNeighbor(otherBranch, target, depth + 1);
			if (bestDist > getSquaredDistance(target, tempPoint))
				bestPoint = tempPoint;
		}
	}
	return bestPoint;
}

// Calculate the distance between two points
template <size_t K, class T>
T KDTree<K, T>::getSquaredDistance(const std::array<T, K>& p1, const std::array<T, K>& p2) {
	T distance = 0;
	for (size_t i = 0; i < K; ++i) {
		T diff = p1[i] - p2[i];
		distance += diff * diff;
	}
	return distance;
}

// private searchRange helper
template <size_t K, class T>
void KDTree<K, T>::searchRange(std::shared_ptr<Node> node, const std::array<T, K>& lowerBound, const std::array<T, K>& upperBound,
	size_t depth, std::vector<std::array<T, K>>& results) {
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
		results.push_back(node->point);

	// Determine which branch to search
	size_t nextDepth = (depth + 1) % K;
	const T& curValue = node->point[depth];
	if (curValue >= lowerBound[depth])
		searchRange(node->left, lowerBound, upperBound, nextDepth, results);
	if (curValue <= upperBound[depth])
		searchRange(node->right, lowerBound, upperBound, nextDepth, results);
}