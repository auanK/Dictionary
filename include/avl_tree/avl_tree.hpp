#pragma once
#include <unicode/coll.h>
#include <unicode/unistr.h>

#include <iostream>
#include <string>

#include "node.hpp"

using namespace icu;

// Functor padrão para comparar tipos genéricos onde suportam operador '<'
template <typename type>
struct default_compare {
    bool operator()(const type& lhs, const type& rhs) const {
        return lhs < rhs;
    }
};

// Functor para comparar strings Unicode usando ICU Collator
// https://unicode-org.github.io/icu/userguide/collation/
struct unicode_compare {
    icu::Collator* collator;  // Ponteiro para o Collator que faz a comparação

    // Construtor que define o Collator
    unicode_compare(icu::Collator* coll) { collator = coll; }

    // Compara duas strings Unicode usando o Collator
    bool operator()(const icu::UnicodeString& lhs,
                    const icu::UnicodeString& rhs) const {
        UErrorCode status = U_ZERO_ERROR;
        return collator->compare(lhs, rhs, status) < 0;
    }
};

// Árvore AVL genérica, caso o compare não seja passado, usa o default_compare
template <typename type, typename compare = default_compare<type>>
class avl_tree {
   private:
    node<type>* _root = nullptr;    // Raiz da árvore
    unsigned int _size = 0;         // Número de nós na árvore
    compare _compare;               // Functor de comparação
    unsigned int _comparisons = 0;  // Número de comparações feitas

    // Limpa a árvore, percorrendo os nós em pós-ordem
    void _clear(node<type>* root) {
        _comparisons++;
        if (root == nullptr) {
            return;
        }

        _clear(root->left);
        _clear(root->right);
        delete root;
    }

    // Retorna a altura de um nó
    int _height(node<type>* n) const {
        if (n == nullptr) {
            return 0;
        }
        return n->height;
    }

    // Calcula o fator de balanceamento de um nó
    int _balance(node<type>* n) const {
        return _height(n->right) - _height(n->left);
    }

    // Rotaciona a subárvore à direita
    node<type>* _right_rotation(node<type>* p) {
        node<type>* u = p->left;
        p->left = u->right;
        u->right = p;

        p->height = 1 + std::max(_height(p->left), _height(p->right));
        u->height = 1 + std::max(_height(u->left), _height(u->right));

        return u;
    }

    // Rotaciona a subárvore à esquerda
    node<type>* _left_rotation(node<type>* p) {
        node<type>* u = p->right;
        p->right = u->left;
        u->left = p;

        p->height = 1 + std::max(_height(p->left), _height(p->right));
        u->height = 1 + std::max(_height(u->left), _height(u->right));

        return u;
    }

    // Insere um nó na árvore
    node<type>* _insert(node<type>* p, type key) {
        // Se chegou em uma folha, insere o nó e incrementa o tamanho da árvore
        //
        _comparisons++;
        if (p == nullptr) {
            _size++;
            return new node<type>(key);
        }

        _comparisons++;
        // Busca binária para encontrar a posição correta do nó
        if (_compare(key, p->key)) {
            p->left = _insert(p->left, key);
        } else if (_compare(p->key, key)) {
            _comparisons++;
            p->right = _insert(p->right, key);
        } else {
            p->freq++;
            return p;
        }

        // Corrige o balanceamento do nó
        p = _fixup_node(p, key);

        return p;
    }

    // Corrige o balanceamento de um nó após uma inserção
    node<type>* _fixup_node(node<type>* p, type key) {
        p->height = 1 + std::max(_height(p->left), _height(p->right));

        int bal = _balance(p);

        _comparisons++;
        if (bal < -1 && _compare(key, p->left->key)) {
            return _right_rotation(p);
        }

        _comparisons++;
        if (bal < -1 && _compare(p->left->key, key)) {
            p->left = _left_rotation(p->left);
            return _right_rotation(p);
        }

        _comparisons++;
        if (bal > 1 && _compare(p->right->key, key)) {
            return _left_rotation(p);
        }

        _comparisons++;
        if (bal > 1 && _compare(key, p->right->key)) {
            p->right = _right_rotation(p->right);
            return _left_rotation(p);
        }

        return p;
    }

    // Remove um nó da árvore
    node<type>* _remove(node<type>* n, type key) {
        _comparisons++;
        if (n == nullptr) {
            return nullptr;
        }

        _comparisons++;
        if (_compare(key, n->key)) {
            n->left = _remove(n->left, key);
        } else if (_compare(n->key, key)) {
            _comparisons++;
            n->right = _remove(n->right, key);
        } else if (n->right == nullptr) {
            _comparisons++;
            node<type>* temp = n->left;
            delete n;
            _size--;
            return temp;
        } else {
            n->right = _remove_successor(n, n->right);
            _size--;
        }

        n = _fixup_deletion(n);

        return n;
    }

    // Remove o sucessor de um nó
    node<type>* _remove_successor(node<type>* n, node<type>* successor) {
        _comparisons++;
        if (successor->left != nullptr) {
            successor->left = _remove_successor(n, successor->left);
        } else {
            n->key = successor->key;
            node<type>* temp = successor->right;
            delete successor;
            return temp;
        }
        successor = _fixup_deletion(successor);
        return successor;
    }

    // Corrige o balanceamento de um nó após uma remoção
    node<type>* _fixup_deletion(node<type>* n) {
        _comparisons++;
        if (n == nullptr) {
            return nullptr;
        }

        n->height = 1 + std::max(_height(n->left), _height(n->right));

        int bal = _balance(n);

        _comparisons++;
        if (bal > 1 && _balance(n->right) >= 0) {
            return _left_rotation(n);
        }

        _comparisons++;
        if (bal > 1 && _balance(n->right) < 0) {
            n->right = _right_rotation(n->right);
            return _left_rotation(n);
        }

        _comparisons++;
        if (bal < -1 && _balance(n->left) <= 0) {
            return _right_rotation(n);
        }

        _comparisons++;
        if (bal < -1 && _balance(n->left) > 0) {
            n->left = _left_rotation(n->left);
            return _right_rotation(n);
        }

        return n;
    }

    // Busca uma chave na árvore
    node<type>* _search(node<type>* n, type key) {
        _comparisons++;
        if (n == nullptr) {
            return nullptr;
        }

        _comparisons++;
        if (!_compare(key, n->key) && !_compare(n->key, key)) {
            return n;
        }

        _comparisons++;
        if (_compare(key, n->key)) {
            return _search(n->left, key);
        }
        return _search(n->right, key);
    }

    // Retorna o nó com a menor chave
    node<type>* _minimum(node<type>* n) {
        while (n->left != nullptr) {
            n = n->left;
        }
        return n;
    }

    // Retorna o nó com a maior chave
    node<type>* _maximum(node<type>* n) {
        while (n->right != nullptr) {
            n = n->right;
        }
        return n;
    }

    // Retorna uma string com as chaves da árvore em ordem alfabética
    std::string _list(node<type>* n) {
        if (n == nullptr) {
            return "";
        }

        std::string list = _list(n->left);

        // Em tempo de compilação, verifica se o tipo é UnicodeString para
        // fazer a conversão correta
        if constexpr (std::is_same<type, icu::UnicodeString>::value) {
            std::string utf8_string;
            n->key.toUTF8String(utf8_string);
            list += utf8_string + " (" + std::to_string(n->freq) + ") \n";
        } else {
            list += n->key + " (" + std::to_string(n->freq) + ") \n";
        }

        list += _list(n->right);

        return list;
    }

    // Imprime a árvore na tela
    void bshow(node<type>* n, std::string heranca) {
        if (n != nullptr && (n->left != nullptr || n->right != nullptr)) {
            bshow(n->right, heranca + "r");
        }
        for (int i = 0; i < (int)heranca.size() - 1; i++) {
            std::cout << (heranca[i] != heranca[i + 1] ? "│   " : "    ");
        }
        if (heranca != "") {
            std::cout << (heranca.back() == 'r' ? "┌───" : "└───");
        }
        if (n == nullptr) {
            std::cout << "#" << std::endl;
            return;
        }

        // Em tempo de compilação, verifica se o tipo é UnicodeString para
        // imprimir corretamente
        if constexpr (std::is_same<type, icu::UnicodeString>::value) {
            std::string utf8_string;
            n->key.toUTF8String(utf8_string);
            std::cout << utf8_string << " (" << n->freq << ")" << std::endl;
        } else {
            std::cout << n->key << " (" << n->freq << ")" << std::endl;
        }

        if (n != nullptr && (n->left != nullptr || n->right != nullptr)) {
            bshow(n->left, heranca + "l");
        }
    }

   public:
    // Construtor com suporte para passar um functor de comparação
    // Se não for passado, usa o default_compare
    avl_tree(compare comp = compare()) : _compare(comp) {
        _root = nullptr;
        _size = 0;
    }

    // Destrutor
    ~avl_tree() { _clear(_root); }

    // Insere um nó na árvore
    void insert(type key) { _root = _insert(_root, key); }

    // Remove um nó da árvore
    void remove(type key) { _root = _remove(_root, key); }

    // Retorna o tamanho da árvore
    unsigned int size() { return _size; }

    // Verifica se a árvore está vazia
    bool empty() const { return _size == 0; }

    // Retorna a altura da árvore
    unsigned int height() const { return _height(_root); }

    // Verifica se a árvore contém uma chave
    bool contains(type key) { return _search(_root, key) != nullptr; }

    // Retorna um vetor com a chave e a frequência do nó com a menor chave
    type* minimum() {
        if (_root == nullptr) {
            return nullptr;
        }

        node<type>* min_node = _minimum(_root);
        return new type[2]{min_node->key, min_node->freq};
    }

    // Retorna um vetor com a chave e a frequência do nó com a maior chave
    type* maximum() {
        if (_root == nullptr) {
            return nullptr;
        }

        node<type>* max_node = _maximum(_root);
        return new type[2]{max_node->key, max_node->freq};
    }

    // Retorna a frequência de uma chave(0 se não existir)
    unsigned int freq(type key) {
        node<type>* n = _search(_root, key);
        if (n == nullptr) {
            return 0;
        }

        return n->freq;
    }

    void att(type key, unsigned int new_freq) {
        node<type>* n = _search(_root, key);
        if (n == nullptr) {
            return;
        }

        if (new_freq == 0) {
            _root = _remove(_root, key);
        } else {
            n->freq = new_freq;
        }
    }

    // Retorna uma string com as chaves da árvore em ordem alfabética
    std::string list() { return _list(_root); }

    // Imprime a árvore na tela em formato de árvore binária
    void show() { bshow(_root, ""); }

    // Retorna o número de comparações feitas
    unsigned int comparisons() const { return _comparisons; }
};
