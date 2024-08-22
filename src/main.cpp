#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <variant>

#include "../include/avl_tree/avl_tree.hpp"
#include "../include/rb_tree/red_black_tree.hpp"
#include "../include/dictionary.hpp"
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
        dictionary<avl_tree<UnicodeString>> dict;
        process_file_and_insert(dict, filename);
    } else if (mode_structure == "dictionary_rb") {
        dictionary<red_black_tree<UnicodeString>> dict;
        process_file_and_insert(dict, filename);
    } else {
        cerr << "Error: Invalid mode of structure" << endl;
        display_usage(argv[0]);
        return 1;
    }

    // Inicia a contagem do tempo
    auto start = high_resolution_clock::now();

    // Lê o arquivo e insere as palavras no dicionário
    stringstream file = read_file("in/" + filename);
    //dict.insert(file);

    // Finaliza a contagem do tempo e calcula a duração
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Tempo de execução: " << duration.count() << "ms" << endl;
    cout << dict.comparisons() << " comparações" << endl;

    // Salva o dicionário no arquivo
    dict.save("out/" + filename, duration);

    return 0;
}
