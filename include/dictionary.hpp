#pragma once

#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <iostream>
#include <sstream>
#include <string>

#include "../src/utils.cpp"

// Dicionário de palavras, template é o tipo de estrutura de dados que será
// utilizada para armazenar as palavras.
template <typename type>
class dictionary {
   private:
    type _dict;  // Dicionário

   public:
    // Construtor
    dictionary() {}

    // Insere uma palavra no dicionário
    void insert(const icu::UnicodeString& word) { _dict.insert(word); }

    // Insere um texto no dicionário, formatando enquanto insere
    void insert_text(icu::UnicodeString text) {
        icu::UnicodeString word;
        int32_t start = 0;
        int32_t end = 0;

        while (end < text.length()) {
            UChar32 c = text.char32At(end);

            // Converte o caractere para minúsculo, se necessário.
            c = u_tolower(c);

            // Verifica se o caractere é alfanumérico ou um hífen.
            if (u_isUAlphabetic(c) || c == '-') {
                // Verifica se o caractere atual é um hífen entre duas palavras
                // alfanuméricas.
                bool is_hyphen_between_words =
                    (c == '-') &&
                    (end > 0 &&
                     u_isUAlphabetic(u_tolower(text.char32At(end - 1)))) &&
                    (end < text.length() - 1 &&
                     u_isUAlphabetic(u_tolower(text.char32At(end + 1))));

                if (is_hyphen_between_words) {
                    end = text.moveIndex32(end, 1);
                } else if (c != '-') {
                    end = text.moveIndex32(end, 1);
                } else {
                    // Substitui hífen não entre palavras por um espaço.
                    text.replace(end, 1, ' ');
                    end = text.moveIndex32(end, 1);
                }
            } else {
                // Extrai a palavra válida e insere no dicionário.
                if (start != end) {
                    text.extractBetween(start, end, word);
                    _dict.insert(word);
                }
                start = end = text.moveIndex32(end, 1);
            }
        }

        // Insere a última palavra.
        if (start != end) {
            text.extractBetween(start, end, word);
            _dict.insert(word);
        }
    }

    // Remove uma palavra do dicionário
    void remove(const icu::UnicodeString& word) { _dict.remove(word); }

    // Limpa o dicionário
    void clear() { _dict.clear(); }

    // Retorna a quantidade de palavras diferentes no dicionário
    unsigned int size() { return _dict.size(); }

    // Verifica se o dicionário está vazio
    bool empty() { return _dict.empty(); }

    // Verifica se uma palavra está no dicionário
    bool contains(const icu::UnicodeString& word) {
        return _dict.contains(word);
    }

    // Atualiza a frequência de uma palavra
    void att(const icu::UnicodeString& word, int att) { _dict.att(word, att); }

    // Retorna a frequência de uma palavra
    unsigned int freq(const icu::UnicodeString& word) {
        return _dict.freq(word);
    }

    // Retorna uma lista das palavras do dicionário com suas frequências
    icu::UnicodeString list() {
        icu::UnicodeString list;
        for (auto it = _dict.begin(); it != _dict.end(); ++it) {
            auto node = *it;
            list += "\"" + node.key + "\" - " +
                    icu::UnicodeString::fromUTF8(std::to_string(node.freq)) +
                    "\n";
        }
        return list;
    }

    void print() {
        std::cout << "Dicionário de palavras:\n\"Palavra\" - Frequência\n";
        std::string list_str;
        this->list().toUTF8String(list_str);
        std::cout << list_str;
    }

    void save(const std::string& filename, std::chrono::milliseconds duration) {
        std::string list_str = "";
        list_str +=
            "Tamanho do dicionário: " + std::to_string(this->size()) + "\n";
        list_str += "Quantidade de comparações: " +
                    std::to_string(this->comparisons()) + "\n";
        list_str +=
            "Tempo para montar a tabela: " + std::to_string(duration.count()) +
            " milissegundos\n\n";
        list_str += "Dicionário de palavras:\n\"Palavra\" - Frequência\n";

        // Convertendo UnicodeString para std::string
        std::string list_content;
        this->list().toUTF8String(list_content);
        list_str += list_content;

        write_file(filename, list_str);
    }

    // Exibe o dicionário em forma de árvore
    // Método temporário (Para testes da AVL e Red-Black)

    void show() { _dict.show(); }

    unsigned int comparisons() { return _dict.comparisons(); }
};
