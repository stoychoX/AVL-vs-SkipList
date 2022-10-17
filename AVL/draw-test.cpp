#include"AVLTree.hpp"

void draw() {
    AVLTree<int> t;

    for (size_t i = 0; i < 20; i++)
        t.push(i);

    t.exportToTex("dump.txt");    
}

// int main() { draw(); }