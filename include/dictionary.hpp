#pragma once

#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <iostream>
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
    void insert(const icu::UnicodeString& word) {
        try {
            auto& value = _dict.search(word);
            value += 1;
        } catch (std::out_of_range& e) {
            _dict.insert(word, 1);
        }
    }

    // Insere um texto no dicionário (o texto já deve estar formatado, contendo
    // apenas espaços e palavras)
    void insert_text(icu::UnicodeString text) {
        // Adiciona um espaço no final do texto para garantir que a última
        // palavra será inserida
        text += " ";

        // Percorre o texto, separando as palavras e inserindo no dicionário
        icu::UnicodeString word;
        for (int i = 0; i < text.length(); i++) {
            // Obtém o caractere na posição i
            UChar32 c = text.char32At(i);
            // Caso o caractere seja um espaço em branco, insere a palavra
            // anterior ao espaço no dicionário
            if (u_isUWhiteSpace(c)) {
                if (!word.isEmpty()) {
                    // Se a palavra não for vazia, insere no dicionário
                    this->insert(word);
                    // Limpa a palavra e prepara para a próxima
                    word.remove();
                }
            } else {
                // Não é um espaço em branco, adiciona o caractere à palavra
                // atual
                word += c;
            }
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
    int search(const icu::UnicodeString& word) { return _dict.search(word); }

    // Retorna uma lista das palavras do dicionário com suas frequências
    icu::UnicodeString list() {
        icu::UnicodeString list;
        for (auto pair : _dict) {
            list += "\"" + pair.first + "\" - " +
                    icu::UnicodeString::fromUTF8(std::to_string(pair.second)) +
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
        std::string comparisons = std::to_string(_dict.comparisons());
        list_str +=
            "Tamanho do dicionário: " + std::to_string(this->size()) + "\n";
        list_str +=
            "Número de comparações: " + std::to_string(_dict.comparisons()) +
            "\n";
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

    unsigned int comparisons() { return _dict.comparisons(); }
};
