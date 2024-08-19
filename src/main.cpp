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

    cout << "Dicionário de palavras:\n";
    string list;
    UnicodeString words = dict.list();
    words.toUTF8String(list);

    cout << list;

    return 0;
}
