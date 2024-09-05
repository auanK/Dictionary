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

// Formata a UnicodeString para minúsculas e remove quaisquer caracteres que não
// sejam alfanuméricos ou hífen

void format_text(UnicodeString &text) {
    // Converte o texto para minúsculas, remove caracteres não alfanuméricos e
    // remove hífens que não sejam entre palavras.

    // Converte o texto para minúsculas
    text.toLower();

    // Itera sobre cada caractere na string `text`.
    for (int i = 0; i < text.length(); i++) {
        // Obtém o caractere atual.
        UChar character = text[i];

        // Verifica se o caractere não é alfanumérico.
        if (!u_isalpha(character)) {
            // Verifica se o caractere atual é um hífen entre duas palavras
            // alfanuméricas.
            bool is_hyphen_between_words =
                (character == '-') && (i > 0 && u_isalpha(text[i - 1])) &&
                (i < text.length() - 1 && u_isalpha(text[i + 1]));

            // Se o hífen não estiver entre duas palavras, substitui o caractere
            // por um espaço.
            if (!is_hyphen_between_words) {
                text.replace(i, 1, ' ');
            }
        }
    }
}

// Retorna uma stringstream com as palavras de um arquivo de texto formatadas
UnicodeString read_file(const string &file_path) {
    // Abre o arquivo
    ifstream input_file(file_path, ios::binary);
    if (!input_file.is_open()) {
        cerr << "Error: Could not open file " << file_path << endl;
        exit(1);
    }

    // Salva o conteúdo do arquivo em uma string e converte para UnicodeString
    string file_content((istreambuf_iterator<char>(input_file)),
                        istreambuf_iterator<char>());
    UnicodeString unicode_content =
        UnicodeString::fromUTF8(StringPiece(file_content));

    // Formata e retorna o conteúdo do arquivo
    format_text(unicode_content);
    return unicode_content;
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
    cerr << "Modos de estrutura: dictionary_avl, dictionary_rb, "
            "dictionary_chained_hash"
         << endl;
}

template <typename dict_type>
void process_and_save_dict(dict_type &dict, const string &filename,
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
    // cout << dict.comparisons() << " comparações" << endl;

    // Salva o dicionário no arquivo
    string out_filename = "out/" + mode_structure + "_" + filename;
    dict.save(out_filename, duration);
}

struct hash_unicode {
    size_t operator()(const UnicodeString &s) const { return s.hashCode(); }
};