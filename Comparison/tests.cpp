#include"AVL-vs-SkipList/AVL/AVLTree.hpp"
#include"AVL-vs-SkipList/SkipList/SkipList.hpp"
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include"AVL-vs-SkipList/Benchmark/Timer.h"

std::string gen_random(const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i)
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];

	return tmp_s;
}

static void loadOxdfordOnSkipList() {
	std::ifstream inFile("oxford-diff.txt");
	SkipList<std::string, 12> toLoad;

	int cnt = 0;

	while (true) {
		std::string word;
		inFile >> word;
		if (inFile.eof()) break;

		toLoad.insert(word);
		++cnt;
	}

	std::cout << cnt;
}

void loadOxdfordOnAVL() {
	Timer t;

	std::ifstream inFile("oxford-diff.txt");
	AVLTree<std::string> toLoad;

	int cnt = 0;

	while (true) {
		std::string word;
		inFile >> word;
		if (inFile.eof()) break;

		toLoad.push(word);
		++cnt;
	}

	std::cout << cnt;
}

void searchHardOnSkipList() {
	std::ifstream inFile("oxford-diff.txt");

	SkipList<std::string, 12> toLoad;

	while (true) {
		std::string word;
		inFile >> word;
		if (inFile.eof()) break;

		toLoad.insert(word);
	}

	Timer t;

	int count = 0;

	for (size_t i = 0; i < 1000000; i++)
		count += toLoad.containsElement(gen_random(12));

	std::cout << count;
}

void searchHardOnAVL() {
	std::ifstream inFile("oxford-diff.txt");

	AVLTree<std::string> toLoad;

	while (true) {
		std::string word;
		inFile >> word;
		if (inFile.eof()) break;

		toLoad.push(word);
	}

	Timer t;

	int count = 0;

	for (size_t i = 0; i < 1000000; i++)
		count += toLoad.exists(gen_random(12));

	std::cout << count;
}

void searchHarryOnSkipList() {
	std::ifstream inFile("oxford-diff.txt");
	SkipList<std::string, 12> toLoad;

	int cnt = 0;

	// Init with oxford
	while (true) {
		std::string word;
		inFile >> word;
		if (inFile.eof()) break;
		toLoad.insert(word);
	}

	std::vector<std::string> c;

	std::ifstream harry("harry.txt");
	// Init with harry
	while (true) {
		std::string word;
		harry >> word;
		if (harry.eof())
			break;

		c.push_back(word);
	}
	Timer t;
	for (size_t i = 0; i < c.size(); i++)
		cnt += toLoad.containsElement(c[i]);
	std::cout << cnt;
}

void searchHarryOnAVL() {
	std::ifstream inFile("oxford-diff.txt");
	AVLTree<std::string> toLoad;

	int cnt = 0;

	// Init with oxford
	while (true) {
		std::string word;
		inFile >> word;
		if (inFile.eof()) break;
		toLoad.push(word);
	}

	std::vector<std::string> c;

	std::ifstream harry("harry.txt");
	// Init with harry
	while (true) {
		std::string word;
		harry >> word;
		if (harry.eof())
			break;

		c.push_back(word);
	}
	Timer t;
	for (size_t i = 0; i < c.size(); i++)
		cnt += toLoad.exists(c[i]);
	std::cout << cnt;
}

int main() {

}