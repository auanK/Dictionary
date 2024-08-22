#include <iostream>
#include <unicode/unistr.h>
#include "red_black_tree.hpp"

using namespace std;

int main() {
    red_black_tree<icu::UnicodeString, unicode_compare> tree;
    tree.insert(icu::UnicodeString("árvore"));
    tree.insert(icu::UnicodeString("árvore"));
    tree.insert(icu::UnicodeString("árvore"));
    tree.insert(icu::UnicodeString("bola"));
    tree.insert(icu::UnicodeString("casa"));
    tree.insert(icu::UnicodeString("dado"));
    tree.insert(icu::UnicodeString("elefante"));
    tree.insert(icu::UnicodeString("foca"));
    tree.insert(icu::UnicodeString("gato"));
    tree.insert(icu::UnicodeString("hipopótamo"));
    tree.insert(icu::UnicodeString("iguana"));
    tree.insert(icu::UnicodeString("jacaré"));
    tree.insert(icu::UnicodeString("leão"));
    tree.insert(icu::UnicodeString("macaco"));
    tree.print();

    tree.remove(icu::UnicodeString("casa"));
    tree.att(icu::UnicodeString("dados"), 0);
    tree.print();

    cout << tree.freq(icu::UnicodeString("árvore")) << endl;

    cout << tree.contains(icu::UnicodeString("árvore")) << endl;
    cout << tree.contains(icu::UnicodeString("casa")) << endl;

    icu::UnicodeString min = tree.minimum();
    icu::UnicodeString max = tree.maximum();
    string min_str, max_str;
    min.toUTF8String(min_str);
    max.toUTF8String(max_str);

    cout << "Minimum: " << min_str << endl;
    cout << "Maximum: " << max_str << endl;
}