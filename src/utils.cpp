#pragma once

#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include "../include/avl_tree/avl_tree.hpp"
#include "../include/dictionary.hpp"

using namespace std;
using namespace icu;
using namespace std::chrono;

// Lê o conteúdo de um arquivo e retorna uma UnicodeString com o conteúdo
icu::UnicodeString read_file(const string &file_path) {
    // Abre o arquivo
    ifstream input_file(file_path, ios::binary);
    if (!input_file.is_open()) {
        cerr << "Error: Could not open file " << file_path << endl;
        exit(1);
    }

    // Salva o conteúdo do arquivo em uma string e converte para UnicodeString
    string file_content((istreambuf_iterator<char>(input_file)),
                        istreambuf_iterator<char>());

    return UnicodeString::fromUTF8(StringPiece(file_content));
}

void write_file(const std::string &file_path, const std::string &content) {
    std::ofstream output_file(file_path);
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        exit(1);
    }

    // Escreve o conteúdo no arquivo
    output_file << content;
    output_file.close();
}

// Função para exibir o uso correto do programa
void display_usage(const char *program_name) {
    cerr << "Uso: " << program_name
         << " <modo_estrutura> <arquivo(deve estar na pasta in)>" << endl;
    cerr << "Modos de estrutura: dictionary_avl, dictionary_rb" << endl;
}

template <typename dict_type>
void process_file_and_insert(dict_type &dict, const string &filename,
                             const string &mode_structure) {
    // Processa o arquivo
    UnicodeString file = read_file("in/" + filename);

    // Inicia a contagem do tempo e insere as palavras no dicionário
    auto start = high_resolution_clock::now();
    dict.insert_text(file);

    // Finaliza a contagem do tempo e calcula a duração
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Tempo de execução: " << duration.count() << "ms" << endl;
    cout << dict.comparisons() << " comparações" << endl;

    // Salva o dicionário no arquivo
    string out_filename = "out/" + mode_structure + "_" + filename;
    dict.save(out_filename, duration);
}