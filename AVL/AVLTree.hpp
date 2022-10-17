#ifndef AVL_TREE_HEADER
#define AVL_TREE_HEADER
#include<cassert>
#include<utility>
#include<fstream>

// BF = height(right) - height(left) \in {-1, 0, 1}

template<class T>
class AVLTree {
private:
	struct Node {
		T data;
		Node* left;
		Node* right;
		int height;

		static void rotateLeft(Node*& subTree) {
			if (!subTree || !subTree->right)
				return;

			Node* originalRight = subTree->right;
			subTree->right = originalRight->left;
			originalRight->left = subTree;
			subTree = originalRight;
		}

		static void rotateRight(Node*& subTree) {
			if (!subTree || !subTree->left)
				return;

			Node* originalLeft = subTree->left;
			subTree->left = originalLeft->right;
			originalLeft->right = subTree;
			subTree = originalLeft;
		}

		static inline int max(int x, int y) {
			return (x > y) ? x : y;
		}

		static void updateHeight(Node* r) {
			if (r)
				r->height = Node::max(Node::getHeight(r->left), Node::getHeight(r->right)) + 1;
		}

		static int getHeight(const Node* r) {
			if (!r)
				return 0;
			return r->height;
		}

		static Node* copyDynamic(const Node* from) {
			if (!from)
				return nullptr;

			return new Node(from->data, copyDynamic(from->left), copyDynamic(from->right), from->height);
		}

		static bool compareNodes(const Node* first, const Node* second) {
			if (!first && !second)
				return true;

			if (!first || !second)
				return false;

			return (first->data == second->data) && compareNodes(first->left, second->left) && compareNodes(first->right, second->right);
		}

		Node(const T& data, Node* l = nullptr, Node* r = nullptr, int h = 1) : data(data), left(l), right(r), height(h) {}
	};

	Node* root;
	int nodesCount;

	int pushRec(const T& elem, Node*& r);

	void freeRec(Node* r);

	void copy(const AVLTree& other);

	int removeRec(Node*& r, const T& elem);
	
	Node* balanceLeftPathAndGetMinNode(Node* rightOfRoot, Node*& minNode);

	void innerVertexRemovalCase(Node*& rootNode, Node*& rightOfRoot);

	bool existRec(const T& elem, const Node* r) const;

	int searchForRightDisbalance(Node*& r);

	int searchForLeftDisbalance(Node*& r);

	void free();

	void recFillFileStream(std::ofstream& outFile, const Node* r) const; 
public:
	class nodeProxy {
	private:
		Node* currNode;

		nodeProxy(Node* r) : currNode(r) {}
	public:
		nodeProxy() = delete;
		nodeProxy(const nodeProxy&) = default;

		nodeProxy(const AVLTree& tree) : currNode(tree.root) {}

		nodeProxy operator++() const {
			if (isValid()) {
				return nodeProxy(currNode->right);
			}
			return nodeProxy(currNode);
		}

		int getHeight() const {
			if (!currNode)
				return 0;

			return currNode->height;
		}

		const T& currData() const {
			return currNode->data;
		}

		const T& operator*() const {
			return currNode->data;
		}

		nodeProxy operator--() const {
			if (isValid()) {
				return nodeProxy(currNode->left);
			}
			return nodeProxy(currNode);
		}

		bool isValid() const {
			return currNode != nullptr;
		}
	};

	AVLTree() : root(nullptr), nodesCount(0) {}

	AVLTree(const T& data) : root(new Node(data)), nodesCount(1) {}

	AVLTree(const AVLTree& other) {
		copy(other);
	}

	AVLTree(AVLTree&& other) noexcept;

	AVLTree& operator=(const AVLTree& other);

	AVLTree& operator=(AVLTree&& other) noexcept;

	bool exists(const T& elem) const;

	int getNodesCount() const;

	int removeElement(const T& elem);

	nodeProxy rootProxy() const;

	int push(const T& elem);

	int getHeight() const;

	bool isEmpty() const;

	void exportToTex(const char* filePath) const;

	~AVLTree();
};

#endif // !AVL_TREE_HEADER

//-1 Провалено вмъкване
// 1 Вмъкването е ок
// 2 Вмъкването е ок и сме направили ротация

template<class T>
int AVLTree<T>::pushRec(const T& elem, Node*& r) {
	int res = 0;

	if (r == nullptr) {
		r = new Node(elem);
		return 1;
	}
	else if (r->data == elem) {
		return -1;
	}
	else if (elem < r->data) {
		res = pushRec(elem, r->left);

		if (res != 1)
			return res;

		if (searchForLeftDisbalance(r) == 1) {
			Node::updateHeight(r);
			return 2;
		}
	}
	else {
		res = pushRec(elem, r->right);

		if (res != 1)
			return res;

		if (searchForRightDisbalance(r) == 1) {
			Node::updateHeight(r);
			return 2;
		}
	}

	Node::updateHeight(r);
	return res;
}

template<class T>
void AVLTree<T>::freeRec(Node* r) {
	if (!r)
		return;
	freeRec(r->left);
	freeRec(r->right);

	delete r;
}

template<class T>
void AVLTree<T>::copy(const AVLTree<T>& other) {
	this->root = Node::copyDynamic(other.root);
	nodesCount = other.nodesCount;
}

template<class T>
int AVLTree<T>::removeRec(Node*& r, const T& elem) {
	if (r == nullptr)
		return -1;

	int res = 1;

	if (r->data == elem) {
		if (!r->left && !r->right) {
			delete r;
			r = nullptr;
			return 1;
		}
		else if ((r->left && !r->right) || (!r->left && r->right)) {
			Node* swapAndDelete = r->left ? r->left : r->right;

			std::swap(r, swapAndDelete);
			
			delete swapAndDelete;

			Node::updateHeight(r);

			return 1;
		}
		else {
			innerVertexRemovalCase(r, r->right);
		}
	}
	else if (r->data > elem) {
		res = removeRec(r->left, elem);
	}
	else {
		res = removeRec(r->right, elem);
	}

	if (res != 1)
		return res;
	Node::updateHeight(r);

	searchForLeftDisbalance(r);
	searchForRightDisbalance(r);

	return res;
}

// Oh no! What about single responsibility principle!!!???
// We do double work here as we first
// find the minNode and then we use backtracking to
// balance the tree.
template<class T>
inline typename AVLTree<T>::Node* AVLTree<T>::balanceLeftPathAndGetMinNode(Node* rightOfRoot, Node*& minNode) {
	if (rightOfRoot->left == nullptr) {
		minNode = rightOfRoot;
		return minNode->right;
	}

	rightOfRoot->left = balanceLeftPathAndGetMinNode(rightOfRoot->left, minNode);

	Node::updateHeight(rightOfRoot);

	// searchForLeftDisbalance(rightOfRoot); -> Never occur as we iterare only left path	
	searchForRightDisbalance(rightOfRoot);

	return rightOfRoot;
}

template<class T>
inline void AVLTree<T>::innerVertexRemovalCase(Node*& rootNode, Node*& rightOfRoot) {
	Node* toDelete = rootNode;

	Node* minNode;

	rightOfRoot = balanceLeftPathAndGetMinNode(rightOfRoot, minNode);

	minNode->left = rootNode->left;
	minNode->right = rootNode->right;
	rootNode = minNode;

	delete toDelete;
}

template<class T>
bool AVLTree<T>::existRec(const T& elem, const Node* r) const {
	if (r == nullptr)
		return false;
	else if (r->data == elem)
		return true;
	else if (r->data < elem)
		return existRec(elem, r->right);
	else
		return existRec(elem, r->left);
}

template<class T>
int AVLTree<T>::searchForLeftDisbalance(Node*& r) {
	int balance = 0;
	int balanceRight = 0;

	balance += r->right ? r->right->height : 0;
	balance -= r->left ? r->left->height : 0;

	if (r->left) {
		balanceRight += r->left->right ? r->left->right->height : 0;
		balanceRight -= r->left->left ? r->left->left->height : 0;
	}

	if (balance == -2) {
		if (balanceRight == 1)
			AVLTree::Node::rotateLeft(r->left);

		AVLTree::Node::rotateRight(r);

		Node::updateHeight(r->left);
		Node::updateHeight(r->right);
		Node::updateHeight(r);
		return 1;
	}

	return 0;
}

template<class T>
int AVLTree<T>::searchForRightDisbalance(Node*& r) {
	int balance = 0;
	int balanceLeft = 0;

	balance += r->right ? r->right->height : 0;
	balance -= r->left ? r->left->height : 0;

	if (r->right) {
		balanceLeft += r->right->right ? r->right->right->height : 0;
		balanceLeft -= r->right->left ? r->right->left->height : 0;
	}

	if (balance == 2) {
		if (balanceLeft == -1)
			AVLTree::Node::rotateRight(r->right);

		AVLTree::Node::rotateLeft(r);

		Node::updateHeight(r->left);
		Node::updateHeight(r->right);
		Node::updateHeight(r);

		return 1;
	}

	return 0;
}

template<class T>
void AVLTree<T>::free() {
	freeRec(root);
}

template<class T>
AVLTree<T>::AVLTree(AVLTree<T>&& other) noexcept {
	this->root = other.root;
	other.root = nullptr;
	nodesCount = other.nodesCount;
}

template<class T>
AVLTree<T>& AVLTree<T>::operator=(const AVLTree<T>& other) {
	if (this != &other) {
		free();
		copy(other);
	}

	return *this;
}

template<class T>
AVLTree<T>& AVLTree<T>::operator=(AVLTree<T>&& other) noexcept {
	if (this != &other) {
		free();

		this->root = other.root;
		other.root = nullptr;
		nodesCount = other.nodesCount;
	}
	return *this;
}

template<class T>
bool AVLTree<T>::exists(const T& elem) const {
	return existRec(elem, root);
}

template<class T>
int AVLTree<T>::getNodesCount() const {
	return nodesCount;
}

template<class T>
int AVLTree<T>::removeElement(const T& elem) {
	int res = removeRec(root, elem);

	if (res != -1)
		nodesCount--;

	return res;
}

template<class T>
typename AVLTree<T>::nodeProxy AVLTree<T>::rootProxy() const {
	return AVLTree::nodeProxy(*this);
}

template<class T>
int AVLTree<T>::push(const T& elem) {
	int res = pushRec(elem, root);

	if (res != -1)
		++nodesCount;

	return res;
}

template<class T>
int AVLTree<T>::getHeight() const {
	return root ? root->height : 0;
}

template<class T>
bool AVLTree<T>::isEmpty() const {
	return (root == nullptr);
}

template<class T>
void AVLTree<T>::recFillFileStream(std::ofstream& outFile, const Node* r) const {
	if(r == nullptr)
		return;

	outFile << "[" << r->data << " ";
	recFillFileStream(outFile, r->left);
	recFillFileStream(outFile, r->right);

	outFile << "]";
}

template<class T>
void AVLTree<T>::exportToTex(const char* filePath) const {
	std::ofstream outFile(filePath, std::ios::trunc);

	outFile << "\\documentclass[tikz,border=10pt]{standalone}" << std::endl;
	outFile << "\\usepackage[linguistics]{forest}" << std::endl;
	outFile << "\\begin{document}" << std::endl;
	outFile << "\\begin{forest}" << std::endl;

	recFillFileStream(outFile, root);

	outFile << "\\end{forest}";
	outFile << "\\end{document}";
}

template<class T>
AVLTree<T>::~AVLTree() {
	free();
}