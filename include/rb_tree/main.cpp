#include <iostream>
#include "red_black_tree.hpp"

using namespace std;

int main() {
    // Cria uma árvore rubro-negra
    red_black_tree<int> tree;

    // Insere elementos na árvore
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(15);
    tree.insert(25);
    tree.insert(35);
    tree.insert(35);

    // Imprime a árvore
    tree.print();

    // Remove um elemento da árvore
    tree.remove(20);

    // Imprime a árvore
    tree.print();

    return 0;
}