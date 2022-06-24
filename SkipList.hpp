/*
* Skip list impl made for passing SDP2 course at FMI Sofia.
*
* Conventions:
*
* Counting levels starts from 1 because of easier indexing/counting.
*
* nullptr is out NIL element, which has value greater than any other.
*
* Why we make NodeBase pnt.
*/

#ifndef SKIP_LIST_HEADER_
#define SKIP_LIST_HEADER_

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

	static Node* copyZeroLever(const Node* start) {
		if (start == nullptr)
			return nullptr;

		Node* toReturn = new Node(start->value, start->levels);

		toReturn->forward[0] = copyZeroLever(start->forward[0]);

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

	SkipList<T, maxLevel>& operator=(const SkipList<T, maxLevel>& other);

	void insert(const T& elem);

	const T& search(const T& elem) const;

	bool removeElement(const T& elem);

	bool containsElement(const T& elem) const;

	bool exceptionSafeSearch(const T& elem, T& result) const;

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
SkipList<T, maxLevel>& SkipList<T, maxLevel>::operator=(const SkipList<T, maxLevel>& other)
{
	if (this != &other) {
		free();
		copyFrom(other);
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
	if (!it)
		throw std::exception("No such element!");

	return static_cast<Node*>(it)->value;
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

	return true;
}

template<class T, unsigned maxLevel>
bool SkipList<T, maxLevel>::containsElement(const T& elem) const {
	try {
		search(elem);
	}
	catch (const std::exception& e) { return false; }
	return true;
}

template<class T, unsigned maxLevel>
bool SkipList<T, maxLevel>::exceptionSafeSearch(const T& elem, T& result) const {
	try {
		result = search(elem);
	}
	catch (const std::exception& e) { return false; }
	
	return true;
}

template<class T, unsigned maxLevel>
void SkipList<T, maxLevel>::print() const {
	for (int i = maxLevel - 1; i >= 0; i--) {
		Node* it = header->forward[i];

		int cnt = 0;

		while (it) {
			std::cout << it->value << " ";
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

	header->forward[0] = copyZeroLever(other.header->forward[0]);

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