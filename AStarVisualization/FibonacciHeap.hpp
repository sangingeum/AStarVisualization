#pragma once
#include <memory>
#include <cmath>
#include <vector>
#include <limits>

// FibonacciHeap Implemented in C++ 
template <class D>
class FibonacciHeap
{
private:
	// Internal node class
	struct Node {
		uint32_t degree{ 0 };
		float key;
		D data;
		bool marked{ false };
		Node* parent{ nullptr };
		Node* left{ this };
		Node* right{ this };
		Node* child{ nullptr };
		Node(float key_, D data_) : key(key_), data(std::move(data_)) {}
	};

	// Private members
	Node* m_top;
	size_t m_size{ 0 };
public:
	// Handle class for the remove and decreaseKey operations
	class Handle {
	private:
		Node* ptr;
		friend FibonacciHeap;
	public:
		Handle(Node* ptr_ = nullptr) : ptr(ptr_) {}
		bool isNull() { return ptr == nullptr; }
	};

	// Constructor & Destructor
	FibonacciHeap() = default;
	~FibonacciHeap() { if (m_top) recursiveFree(m_top); }
	// Operations
	D top() const;
	Handle push(float key, D data);
	void pop();
	void remove(Handle handle);
	bool decreaseKey(Handle handle, float newKey);
	bool empty() const;
	size_t size() const;
	void clear();
	void merge(FibonacciHeap<D>& other);

private:
	void appendToRootList(Node* node);
	static void appendToList(Node* root, Node* node);
	void removeFromList(Node* node);
	void consolidate();
	void childification(Node* parent, Node* toBeChild);
	void heapLink(Node* parent, Node* toBeChild);
	void recursiveFree(Node* node);
	void cut(Node* parent, Node* child);
	void cascadingCut(Node* node);
};



// The returned handle can be used for the decreaseKey and remove methods
template<class D>
typename FibonacciHeap<D>::Handle FibonacciHeap<D>::push(float key, D data) {
	Node* node = new Node(key, std::move(data));
	if (m_top) { // heap is not empty
		appendToRootList(node);
		if (node->key < m_top->key) {
			m_top = node;
		}
	}
	else { // heap is empty
		m_top = node;
	}
	++m_size;
	return Handle(node);
}

// Get the data with the lowest key
template<class D>
D FibonacciHeap<D>::top() const {
	return m_top->data;
}

// Pop a node with the lowest key
template<class D>
void FibonacciHeap<D>::pop() {
	auto oldTop = m_top;
	if (oldTop) {
		// Append children of the top node to the root list
		auto directChild = oldTop->child;
		auto child = directChild;
		if (child) {
			while (true) {
				auto left = child->left;
				appendToRootList(child);
				child = left;
				if (child == directChild)
					break;
			}
		}
		// Remove oldTop from the root list
		removeFromList(oldTop);
		// If oldTop is the last node
		if (oldTop == oldTop->right) {
			m_top = nullptr;
		}
		// If not
		else {
			m_top = oldTop->right;
			consolidate();
		}
		--m_size;

		delete oldTop;
	}
}

template<class D>
bool FibonacciHeap<D>::empty() const {
	return m_top == nullptr;
}

template<class D>
size_t FibonacciHeap<D>::size() const {
	return m_size;
}

template<class D>
void FibonacciHeap<D>::clear() {
	if (m_top) {
		recursiveFree(m_top);
		m_top = nullptr;
		m_size = 0;
	}
}

// Remove the node associated with the given handle
template<class D>
void FibonacciHeap<D>::remove(Handle handle) {
	decreaseKey(handle, std::numeric_limits<float>::lowest());
	pop();
}

// Decrease the key value of the the node associated with the given handle
template<class D>
bool FibonacciHeap<D>::decreaseKey(Handle handle, float newKey) {
	Node* node = handle.ptr;
	if (node->key > newKey) {
		node->key = newKey;
		auto parent = node->parent;
		if (parent && parent->key > newKey) {
			cut(parent, node);
			cascadingCut(parent);
		}
		if (newKey < m_top->key) {
			m_top = node;
		}

		return true;
	}
	return false;
}

// Merge the given heap into this heap
// The given heap will be empty after this operation
template<class D>
void FibonacciHeap<D>::merge(FibonacciHeap<D>& other) {
	if (other.m_top) {
		if (m_top) {
			std::vector<Node*> rootList;
			auto cur = other.m_top;
			if (cur) {
				while (true) {
					rootList.push_back(cur);
					cur = cur->left;
					if (cur == other.m_top)
						break;
				}
			}
			for (auto& node : rootList)
				appendToRootList(node);

			if (other.m_top->key < m_top->key) {
				m_top = other.m_top;
			}
		}
		else {
			m_top = other.m_top;
		}
		m_size += other.m_size;
		other.m_top = nullptr;
		other.m_size = 0;
	}
}

// This function assumes that m_top is not null
template<class D>
void FibonacciHeap<D>::appendToRootList(Node* node) {
	node->parent = nullptr;
	node->marked = false;
	node->left = m_top->left;
	node->right = m_top;
	m_top->left->right = node;
	m_top->left = node;
}

template<class D>
void FibonacciHeap<D>::appendToList(Node* root, Node* node) {
	node->left = root->left;
	node->right = root;
	root->left->right = node;
	root->left = node;
}

template<class D>
void FibonacciHeap<D>::removeFromList(Node* node) {
	node->right->left = node->left;
	node->left->right = node->right;
}


template<class D>
void FibonacciHeap<D>::consolidate() {
	auto degree = static_cast<uint32_t>(std::floor(std::log10(m_size) / std::log10(1.618)));
	std::vector<Node*> degreeNodeArray(degree + 1);
	// prevent loop invalidation
	auto cur = m_top;
	std::vector<Node*> loop;
	if (cur) {
		while (true) {
			loop.push_back(cur);
			cur = cur->left;
			if (cur == m_top)
				break;
		}
	}
	// Fill in degreeNodeArray
	for (auto& cur : loop) {
		auto curDegree = cur->degree;
		while (degreeNodeArray[curDegree]) {
			auto sameDegreeNode = degreeNodeArray[curDegree];
			if (sameDegreeNode->key < cur->key) {
				std::swap(sameDegreeNode, cur);
			}
			// Attach sameDegreeNode to cur as a child
			heapLink(cur, sameDegreeNode);
			degreeNodeArray[curDegree] = nullptr;
			++curDegree;
		}
		degreeNodeArray[curDegree] = cur;
	}
	//
	m_top = nullptr;
	for (auto& node : degreeNodeArray) {
		if (node) {
			node->left = node->right = node;
			if (m_top) {
				appendToRootList(node);
				if (node->key < m_top->key) {
					m_top = node;
				}
			}
			else {
				m_top = node;
			}
		}
	}
}

template<class D>
void FibonacciHeap<D>::childification(Node* parent, Node* toBeChild) {
	toBeChild->left = toBeChild->right = toBeChild;
	if (parent->child)
		appendToList(parent->child, toBeChild);
	else
		parent->child = toBeChild;
	toBeChild->parent = parent;
	toBeChild->marked = false;
	++(parent->degree);
}

// Make `toBeChild` a child of `parent`
template<class D>
void FibonacciHeap<D>::heapLink(Node* parent, Node* toBeChild) {
	removeFromList(toBeChild);
	childification(parent, toBeChild);
}

template<class D>
void FibonacciHeap<D>::recursiveFree(Node* node) {
	node->right->left = nullptr;
	while (node) {
		if (node->child)
			recursiveFree(node->child);
		auto left = node->left;
		delete node;
		node = left;
	}
}

// Cut `child` from the child lisf of `parent` and move it to the root list.
template<class D>
void FibonacciHeap<D>::cut(Node* parent, Node* child) {
	if (parent->child == child) {
		if (child->right == child)
			parent->child = nullptr;
		else
			parent->child = child->right;
	}
	removeFromList(child);
	--(parent->degree);
	appendToRootList(child);
}

template<class D>
void FibonacciHeap<D>::cascadingCut(Node* node) {
	auto parent = node->parent;
	if (parent) {
		if (node->marked) {
			cut(parent, node);
			cascadingCut(parent);
		}
		else {
			node->marked = true;
		}
	}
}

