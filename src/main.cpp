#include <unicode/locid.h>
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <iostream>
#include <sstream>

#include "../include/avl_tree/avl_tree.hpp"
#include "utils.cpp"

using namespace std;
using namespace icu;

// Teste da árvore AVL com strings Unicode
int main() {
    // Define o Locale para pt-BR (Português do Brasil)
    Locale brazilian_portuguese("pt", "BR");

    // Cria um collator para comparar strings Unicode com Locale pt-BR
    UErrorCode status = U_ZERO_ERROR;
    unique_ptr<Collator> collator(Collator::createInstance(brazilian_portuguese, status));

    // Verifica se houve erro ao criar o collator
    if (U_FAILURE(status)) {
        cerr << "Erro ao criar o collator: " << u_errorName(status) << endl;
        return 1;
    }

    // Cria uma árvore AVL do tipo UnicodeString com o collator criado
    avl_tree<UnicodeString, unicode_compare> tree(unicode_compare(collator.get()));

    // Lê as palavras de um arquivo de texto e insere na árvore
    stringstream ss = read_file("in/file.txt");
    string word;
    while (ss >> word) {
        UnicodeString uword = UnicodeString::fromUTF8(StringPiece(word));
        tree.insert(uword);
    }

    tree.show();
}
