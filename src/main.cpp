#include <iostream>

#include "../include/avl_tree/avl_tree.hpp"
#include "../include/dictionary.hpp"
#include "utils.cpp"

using namespace std;
using namespace icu;

int main() {
    UErrorCode status = U_ZERO_ERROR;
    Collator* collator = Collator::createInstance(status);

    if (U_FAILURE(status)) {
        cerr << "Erro ao criar o Collator: " << u_errorName(status) << endl;
        return 1;
    }

    dictionary<avl_tree<UnicodeString, unicode_compare>> dict(collator);

    stringstream file = read_file("in/file.txt");
    string word;
    while (file >> word) {
        dict.insert(UnicodeString::fromUTF8(StringPiece(word)));
    }

    string list;
    dict.list().toUTF8String(list);
    cout << list << endl;
    cout << dict.comparisons() << endl;

    delete collator;

    return 0;
}
