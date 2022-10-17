// Simple way to draw small trees on latex
// Simply copy the dump.txt code and paste it in overleaf

#include"AVLTree.hpp"

void draw() {
    AVLTree<int> t;

    for (size_t i = 0; i < 20; i++)
        t.push(i);

    t.exportToTex("dump.txt");    
}

// int main() { draw(); }