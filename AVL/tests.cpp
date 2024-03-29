//www.github.com/doctest
#include "AVLTree.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include"../../doctest.h"
#include<cmath>
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include<algorithm>
#include<string>

template<class T>
bool correctHeight(const AVLTree<T>& t) {
	if (t.getHeight() == 0)
		return true;

	double lowerBound = log2(t.getNodesCount());
	double upperBound = 2 * log2(t.getNodesCount() + 1) - 1;

	return lowerBound <= t.getHeight() && t.getHeight() <= upperBound;
}

template<class T>
bool isAVL(const typename AVLTree<T>::NodeProxy& t) {
	if (!t.isValid())
		return true;
	int lHeight = (--t).getHeight();
	int rHeight = (++t).getHeight();

	return std::abs(rHeight - lHeight) < 2 && isAVL<T>(++t) && isAVL<T>(--t);
}

TEST_CASE("test on big tree") {
	int nodesCount = 1000000;

	AVLTree<int> t;

	for (int i = 0; i < nodesCount; i++)
		t.push((rand() % (2 * nodesCount)) + 1);

	CHECK(isAVL<int>(t.rootProxy()));
	CHECK(correctHeight(t));
}

TEST_CASE("test BST property and correct heigth on 100 random trees") {
	for (int i = 0; i < 100; i++) {
		AVLTree<int> t;
		int randNumberOfNodes = rand() % 10000;

		for (int i = 0; i < randNumberOfNodes; i++)
			t.push(rand() % 10000);
		CHECK(isAVL<int>(t.rootProxy()));
		CHECK(correctHeight(t));
	}
}

TEST_CASE("check on one element tree") {
	AVLTree<int> t;
	t.push(1);

	CHECK(isAVL<int>(t.rootProxy()));
	CHECK(correctHeight(t));
}

TEST_CASE("check if find works") {
	AVLTree<int> t;

	int toFild = rand() % 100 + 1;

	for (int i = 0; i < 100; i++)
		t.push(rand() % 1000 + 1);

	t.push(toFild);

	CHECK(t.exists(toFild));
	CHECK(t.exists(0) == false);
	CHECK(correctHeight(t));
}

TEST_CASE("check if nodes count is correct") {
	AVLTree<int> t;
	int len = rand() % 100;

	for (int i = 0; i < len; i++)
		t.push(i);

	CHECK(t.getNodesCount() == len);
	CHECK(correctHeight(t));
}

TEST_CASE("check removing one element") {
	AVLTree<int> t;
	for (int i = 0; i < 10; i++)
		t.push(i);

	CHECK(t.exists(1));

	t.removeElement(1);

	CHECK(t.exists(1) == false);
}

TEST_CASE("check avl property after removing element with rotation") {
	AVLTree<int> t;

	for (int i = 1; i < 10; i++)
		t.push(i);

	t.removeElement(6);

	CHECK(isAVL<int>(t.rootProxy()));
	CHECK(correctHeight(t));
}

TEST_CASE("check avl property after removing a lot of elements") {
	AVLTree<int> t;

	for (int i = 0; i < 100; i++)
		t.push(i);

	for (int i = 0; i < 100; i += 3) {
		t.removeElement(i);
		CHECK(isAVL<int>(t.rootProxy()));
		CHECK(correctHeight(t));
	}
}

TEST_CASE("check nodes count") {
	AVLTree<int> t;

	int length = 10000;

	std::vector<int> v;

	v.reserve(length);

	for (int i = 0; i < length; i++)
		v.emplace_back(i);

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	std::shuffle(v.begin(), v.end(), std::default_random_engine(seed));

	for (int i = 0; i < length; i++)
		t.push(v[i]);

	int cnt = length;

	CHECK(t.getNodesCount() == length);

	for (int i = 0; i < length; i++) {
		t.removeElement(v[i]);

		CHECK(isAVL<int>(t.rootProxy()));
		CHECK(correctHeight(t));
		CHECK(--cnt == t.getNodesCount());
	}
}

TEST_CASE("Some more trees") {
	AVLTree<char> tree;

	for (size_t i = 0; i < 10000; i++)
	{
		tree.push(rand() % 256);
		CHECK(isAVL<char>(tree.rootProxy()));
		CHECK(correctHeight(tree));
	}
}

TEST_CASE("different-types") {
	AVLTree<unsigned> noSignTree;

	for (unsigned i = 0; i < 100; i++)
		noSignTree.push(i);

	CHECK(noSignTree.getNodesCount() == 100);

	AVLTree<std::string> stringTree;
}

TEST_CASE("basic test on iterator") {
	AVLTree<int> t;

	const int ELEMENTS = 10000;

	for(int i = 0; i < ELEMENTS; i++) {
		t.push(rand());
	}

	CHECK(std::is_sorted(t.begin(), t.end()));
}