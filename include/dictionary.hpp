#pragma once

#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <iostream>
#include <sstream>
#include <string>

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

    // Remove uma palavra do dicionário
    void remove(const icu::UnicodeString& word) { _dict.remove(word); }

    // Retorna a quantidade de palavras diferentes no dicionário
    unsigned int size() { return _dict.size(); }

    // Verifica se o dicionário está vazio
    bool empty() { return _dict.empty(); }

    // Verifica se uma palavra está no dicionário
    bool contains(const icu::UnicodeString& word) {
        return _dict.contains(word);
    }

    // Retorna a menor palavra do dicionário(lexicograficamente)
    icu::UnicodeString minimun() { return _dict.minimun(); }

    // Retorna a maior palavra do dicionário(lexicograficamente)
    icu::UnicodeString maximum() { return _dict.maximum(); }

    // Atualiza a frequência de uma palavra
    void att(const icu::UnicodeString& word, int att) { _dict.att(word, att); }

    // Retorna a frequência de uma palavra
    unsigned int freq(const icu::UnicodeString& word) {
        return _dict.freq(word);
    }

    // Retorna uma lista das palavras do dicionário
    icu::UnicodeString list() {
        std::string dict_list = _dict.list();
        return icu::UnicodeString::fromUTF8(dict_list);
    }

    // Exibe o dicionário em forma de árvore
    // Método temporário (Para testes da AVL e Red-Black)
    void show() { _dict.show(); }

    unsigned int comparisons() { return _dict.comparisons(); }
};
