#include <iostream>

#include "../include/avl_tree/avl_tree.hpp"
#include "../include/dictionary.hpp"
#include "utils.cpp"

using namespace std;
using namespace icu;

int main() {
    dictionary<avl_tree<UnicodeString, unicode_compare>> dict;

    stringstream file = read_file("in/memorias_postumas_de_braz_cubas.txt");
    string word;
    while (file >> word) {
        dict.insert(UnicodeString::fromUTF8(StringPiece(word)));
    }

    avl_tree<UnicodeString, unicode_compare> tree;

    tree.insert(UnicodeString("casa"));
    tree.insert(UnicodeString("casa"));
    tree.insert(UnicodeString("árvore"));
    tree.insert(UnicodeString("floresta"));
    tree.insert(UnicodeString("carro"));

    for (auto it = tree.begin(); it != tree.end(); ++it) {
        auto node = *it;
        string utf8_key;
        node.key.toUTF8String(utf8_key);
        cout << "Chave: " << utf8_key << ", Frequência: " << node.freq << endl;
    }

    return 0;
}
