#include"../SkipList/SkipList.hpp"
#include"../AVL/AVLTree.hpp"
#include "../Benchmark/Timer.h"

#include<benchmark/benchmark.h>

#include<iostream>
#include<fstream>
#include<vector>
#include<string>

const int ELEMS = 100000;

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

static void loadOxdfordOnSkipList(benchmark::State& state) {
	for(auto x : state){
		std::ifstream inFile("oxford-diff.txt");
		SkipList<std::string, 12> toLoad;

		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			inFile >> word;
			toLoad.insert(word);
		}
	}
}

static void loadOxdfordOnAVL(benchmark::State& state) {
	for(auto x : state){
		std::ifstream inFile("oxford-diff.txt");
		AVLTree<std::string> toLoad;

		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			inFile >> word;
			if (inFile.eof()) break;

			toLoad.push(word);
		}
	}
}

static void searchHardOnSkipList(benchmark::State& state) {
	for(auto x : state){
		std::ifstream inFile("oxford-diff.txt");

		SkipList<std::string, 12> toLoad;

		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			inFile >> word;
			if (inFile.eof()) break;

			toLoad.insert(word);
		}

		for (size_t i = 0; i < 1000000; i++)
			benchmark::DoNotOptimize(toLoad.containsElement(gen_random(12)));
	}
}

static void searchHardOnAVL(benchmark::State& state) {
	for(auto x : state){
		std::ifstream inFile("oxford-diff.txt");

		AVLTree<std::string> toLoad;

		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			inFile >> word;
			if (inFile.eof()) break;

			toLoad.push(word);
		}

		for (size_t i = 0; i < 1000000; i++)
			benchmark::DoNotOptimize(toLoad.exists(gen_random(12)));
	}
}

static void searchHarryOnSkipList(benchmark::State& state) {
	for(auto x : state){
		std::ifstream inFile("oxford-diff.txt");
		SkipList<std::string, 12> toLoad;

		// Init with oxford
		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			inFile >> word;
			if (inFile.eof()) break;
			toLoad.insert(word);
		}

		std::vector<std::string> c;

		std::ifstream harry("harry.txt");
		// Init with harry
		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			harry >> word;
			if (harry.eof())
				break;

			c.push_back(word);
		}
		for (size_t i = 0; i < c.size(); i++)
			benchmark::DoNotOptimize(toLoad.containsElement(c[i]));
	}
}

static void searchHarryOnAVL(benchmark::State& state) {
	for(auto x : state){
		std::ifstream inFile("oxford-diff.txt");
		AVLTree<std::string> toLoad;

		// Init with oxford
		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			inFile >> word;
			if (inFile.eof()) break;
			toLoad.push(word);
		}

		std::vector<std::string> c;

		std::ifstream harry("harry.txt");
		// Init with harry
		for(int i = 0; i < ELEMS; ++i){
			std::string word;
			harry >> word;
			if (harry.eof())
				break;

			c.push_back(word);
		}

		for (size_t i = 0; i < c.size(); i++)
			benchmark::DoNotOptimize(toLoad.exists(c[i]));
		}
}

BENCHMARK(loadOxdfordOnSkipList);
BENCHMARK(loadOxdfordOnAVL);
BENCHMARK(searchHardOnSkipList);
BENCHMARK(searchHardOnAVL);
BENCHMARK(searchHarryOnSkipList);
BENCHMARK(searchHarryOnAVL);

BENCHMARK_MAIN();