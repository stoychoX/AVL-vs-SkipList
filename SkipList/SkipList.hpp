/*
* Skip list impl made for passing SDP2 course at FMI Sofia.
*
* Conventions:
*
* Counting levels starts from 1 because of easier indexing/counting.
*
* nullptr is our NIL element, which has value greater than any other.
*
* We make NodeBase class so we dont force having T in our header node as T might be "expencive".
*/

#ifndef SKIP_LIST_HEADER_
#define SKIP_LIST_HEADER_
#include<iostream>
#include<stack>

template<class T, unsigned maxLevel = 6>
class SkipList {
private:
	class Node;

	class NodeBase {
	public:
		Node** forward;
		unsigned levels;

		NodeBase(unsigned createWithLevels) {
			if (createWithLevels > maxLevel)
				createWithLevels = maxLevel;

			levels = createWithLevels;

			forward = new Node * [levels];

			for (size_t i = 0; i < levels; i++)
				forward[i] = nullptr;
		}

		NodeBase(const NodeBase&) = delete;
		NodeBase& operator=(const NodeBase&) = delete;

		~NodeBase() { free(); }

	private:
		void free() {
			delete[] forward;
		}
	};

	class Node : public NodeBase {
	public:
		T value;

		Node(const T& data, unsigned levels) : NodeBase(levels), value(data) {}
	};

	static unsigned generateRandomLevel() {
		int toReturn = 1;

		while (rand() % 2 && toReturn != maxLevel) {
			++toReturn;
		}

		return toReturn;
	}

	// Using stack approach bc it breaks
	// when I try to copy list with over 25000 elements.
	// Using this one we easily copy 100000 element list
	static Node* copyZeroLevelStack(const Node* start) {
		std::stack<const Node*> s;

		const Node* it = start;

		while (it) {
			s.push(it);
			it = it->forward[0];
		}

		Node* toReturn = nullptr;

		while (!s.empty()) {
			Node* toAdd = new Node(s.top()->value, s.top()->levels);

			toAdd->forward[0] = toReturn;
			toReturn = toAdd;

			s.pop();
		}

		return toReturn;
	}

	static void pushPath(NodeBase*& here, Node*& path, int way) {
		NodeBase* it = here;
		while (it && it->forward[way]) {
			it = it->forward[way];
		}

		if (it)
			it->forward[way] = path;
	}

public:
	SkipList();

	SkipList(const SkipList<T, maxLevel>&);
	SkipList(SkipList<T, maxLevel>&&) noexcept;

	SkipList<T, maxLevel>& operator=(const SkipList<T, maxLevel>& other);
	SkipList<T, maxLevel>& operator=(SkipList<T, maxLevel>&&) noexcept;

	void insert(const T& elem);

	const T& search(const T& elem) const;

	bool removeElement(const T& elem);

	bool containsElement(const T& elem) const;

	bool exceptionSafeSearch(const T& elem, T& result) const;

	size_t elementsCount() const;

	bool empty() const;

	void print() const;

	~SkipList();
private:
	size_t size;
	unsigned level;

	NodeBase* header;

	void free();
	void copyFrom(const SkipList<T, maxLevel>&);
};
#endif

template<class T, unsigned maxLevel>
SkipList<T, maxLevel>::SkipList() {
	size = 0;
	level = 1;

	header = new NodeBase(maxLevel);
}

template<class T, unsigned maxLevel>
SkipList<T, maxLevel>::SkipList(const SkipList<T, maxLevel>& other) {
	copyFrom(other);
}

template<class T, unsigned maxLevel>
SkipList<T, maxLevel>::SkipList(SkipList<T, maxLevel>&& other) noexcept {
	this->header = other.header;
	other.header = nullptr;

	this->size = other.size;
	this->level = other.level;
}

template<class T, unsigned maxLevel>
SkipList<T, maxLevel>& SkipList<T, maxLevel>::operator=(const SkipList<T, maxLevel>& other)
{
	if (this != &other) {
		free();
		copyFrom(other);
	}
	return *this;
}

template<class T, unsigned maxLevel>
SkipList<T, maxLevel>& SkipList<T, maxLevel>::operator=(SkipList<T, maxLevel>&& other) noexcept {
	if (this != &other) {
		this->header = other.header;
		other.header = nullptr;

		this->size = other.size;
		this->level = other.level;
	}
	return *this;
}

template<class T, unsigned maxLevel>
void SkipList<T, maxLevel>::insert(const T& elem) {
	NodeBase* update[maxLevel];

	NodeBase* iterate = header;

	for (int i = maxLevel - 1; i >= 0; i--) {
		while (iterate->forward[i] && iterate->forward[i]->value < elem) {
			iterate = iterate->forward[i];
		}

		update[i] = iterate;
	}

	iterate = iterate->forward[0];

	Node* iterateNodeCast = nullptr;

	if (iterate)
		iterateNodeCast = static_cast<Node*>(iterate);

	unsigned newLevel = generateRandomLevel();

	if (newLevel > level) {
		for (size_t i = level; i < newLevel; i++)
			update[i] = header;

		level = newLevel;
	}

	iterateNodeCast = new Node(elem, newLevel);

	for (size_t i = 0; i < newLevel; i++) {
		iterateNodeCast->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = iterateNodeCast;
	}

	++size;
}

template<class T, unsigned maxLevel>
const T& SkipList<T, maxLevel>::search(const T& elem) const {
	NodeBase* it = header;

	for (int i = maxLevel - 1; i >= 0; --i) {
		while (it->forward[i] && it->forward[i]->value < elem) {
			it = it->forward[i];
		}
	}
	it = it->forward[0];

	if (it) {
		it = static_cast<Node*>(it);
		if (it->value == elem)
			return it->value;
	}

	throw std::exception("No such element!");
}

template<class T, unsigned maxLevel>
bool SkipList<T, maxLevel>::removeElement(const T& elem) {
	NodeBase* update[maxLevel];

	NodeBase* it = header;

	for (int i = maxLevel - 1; i >= 0; --i) {
		while (it->forward[i] && it->forward[i]->value < elem) {
			it = it->forward[i];
		}

		update[i] = it;
	}
	it = it->forward[0];

	if (!it)
		return false;

	for (size_t i = 0; i < maxLevel; i++) {
		if (update[i]->forward[i] != it)
			break;

		update[i]->forward[i] = it->forward[i];
	}

	delete it;

	while (level > 0 && header->forward[level] == nullptr) { --level; }

	--size;

	return true;
}

template<class T, unsigned maxLevel>
bool SkipList<T, maxLevel>::containsElement(const T& elem) const {
	NodeBase* it = header;

	for (int i = maxLevel - 1; i >= 0; --i) {
		while (it->forward[i] && it->forward[i]->value < elem) {
			it = it->forward[i];
		}
	}
	it = it->forward[0];
	return (it && static_cast<Node*>(it)->value == elem);
}

template<class T, unsigned maxLevel>
bool SkipList<T, maxLevel>::exceptionSafeSearch(const T& elem, T& result) const {
	NodeBase* it = header;

	for (int i = maxLevel - 1; i >= 0; --i) {
		while (it->forward[i] && it->forward[i]->value < elem) {
			it = it->forward[i];
		}
	}
	it = it->forward[0];

	if (it) {
		it = static_cast<Node*>(it);
		if (it->value == elem)
			result = it->value;

		return true;
	}

	return false;
}

template<class T, unsigned maxLevel>
inline size_t SkipList<T, maxLevel>::elementsCount() const {
	return size;
}

template<class T, unsigned maxLevel>
inline bool SkipList<T, maxLevel>::empty() const {
	return (size == 0);
}

template<class T, unsigned maxLevel>
void SkipList<T, maxLevel>::print() const {
	for (int i = maxLevel - 1; i >= 0; i--) {
		Node* it = header->forward[i];

		int cnt = 0;

		while (it) {
			// std::cout << it->value << " ";
			++cnt;
			it = it->forward[i];
		}

		std::cout << "#" << cnt << "#" << std::endl;
	}
	std::cout << std::endl;
}

template<class T, unsigned maxLevel>
SkipList<T, maxLevel>::~SkipList() {
	free();
}

template<class T, unsigned maxLevel>
void SkipList<T, maxLevel>::free() {
	Node* it = header->forward[0];

	while (it) {
		Node* capture = it;
		it = it->forward[0];
		delete capture;
	}

	delete header;
}

template<class T, unsigned maxLevel>
void SkipList<T, maxLevel>::copyFrom(const SkipList<T, maxLevel>& other) {
	size = other.size;
	level = other.level;

	header = new NodeBase(maxLevel);

	header->forward[0] = copyZeroLevelStack(other.header->forward[0]);

	Node* currentIterator = header->forward[0];
	Node* otherIterator = other.header->forward[0];

	Node** otherPaths[maxLevel - 1];

	for (size_t i = 0; i < maxLevel - 1; i++)
		otherPaths[i] = &other.header->forward[i + 1];

	while (otherIterator) {
		int j = 1;

		while (j < maxLevel && (otherIterator == (*otherPaths[j - 1]))) {
			pushPath(header, currentIterator, j);
			(*otherPaths[j - 1]) = (*otherPaths[j - 1])->forward[j];
			j++;
		}

		currentIterator = currentIterator->forward[0];
		otherIterator = otherIterator->forward[0];
	}
}
