#include <chrono>
#include <fstream>
#include <iostream>

#include "../include/avl_tree/avl_tree.hpp"
#include "../include/dictionary.hpp"
#include "../include/hash_table/chained_hash_table.hpp"
#include "../include/rb_tree/red_black_tree.hpp"
#include "utils.cpp"

using namespace std;

int main(int argc, char* argv[]) {
    // Verifica se o número de argumentos está correto
    if (argc != 3) {
        display_usage(argv[0]);
        return 1;
    }
    // Salva o modo de estrutura e o nome do arquivo
    string mode_structure = argv[1];
    string filename = argv[2];

    if (mode_structure == "dictionary_avl") {
        dictionary<avl_tree<UnicodeString, int, unicode_compare>> dict;
        process_and_save_dict(dict, filename, mode_structure);
    } else if (mode_structure == "dictionary_rb") {
        dictionary<red_black_tree<UnicodeString, int, unicode_compare>> dict;
        process_and_save_dict(dict, filename, mode_structure);

    } else if (mode_structure == "dictionary_chained_hash") {
        dictionary<chained_hash_table<UnicodeString, int, hash_unicode,
                                      unicode_compare>>
            dict;
        process_and_save_dict(dict, filename, mode_structure);
    } else {
        cerr << "Error: Invalid mode of structure" << endl;
        display_usage(argv[0]);
        return 1;
    }

    return 0;
}
