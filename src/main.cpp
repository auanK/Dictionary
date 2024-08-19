#include <fstream>
#include <iostream>
#include <sstream>

#include "../include/avl_tree/avl_tree.hpp"
#include "../include/dictionary.hpp"
#include "utils.cpp"

using namespace std;
using namespace icu;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <modo_estrutura> <arquivo>" << endl;
        return 1;
    }

    string mode_structure = argv[1];
    string filename = argv[2];

    if (mode_structure != "dictionary_avl") {
        cerr << "Estrutura não suportada: " << mode_structure << endl;
        return 1;
    }

    dictionary<avl_tree<UnicodeString, unicode_compare>> dict;

    stringstream file = read_file(filename);
    string word;
    while (file >> word) {
        dict.insert(UnicodeString::fromUTF8(StringPiece(word)));
    }

    cout << "Dicionário de palavras:" << endl;
    string list;
    UnicodeString words = dict.list();
    words.toUTF8String(list);

    cout << list;

    return 0;
}
