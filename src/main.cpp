#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

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

    // Verifica se a estrutura fornecida é válida
    if (!is_valid_structure(mode_structure)) {
        cerr << "Estrutura não suportada: " << mode_structure << endl;
        return 1;
    }

    dictionary<red_black_tree<UnicodeString, unicode_compare>> dict;

    // Inicia a contagem do tempo
    auto start = high_resolution_clock::now();

    // Lê o arquivo e insere as palavras no dicionário
    stringstream file = read_file("in/" + filename);
    dict.insert(file);

    // Finaliza a contagem do tempo e calcula a duração
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    // Salva o dicionário no arquivo
    // dict.save("out/" + filename, duration);

    return 0;
}
