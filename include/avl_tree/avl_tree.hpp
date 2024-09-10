#pragma once
#include <unicode/coll.h>
#include <unicode/unistr.h>

#include <iostream>
#include <stack>
#include <string>

#include "../compare.hpp"
#include "node.hpp"

using namespace icu;

template <typename key_t, typename value_t, typename compare = std::less<key_t>>
class avl_tree {
   private:
    node<key_t, value_t>* _root = nullptr;  // Raiz da árvore
    unsigned int _size = 0;                 // Número de nós na árvore
    compare _compare;                       // Functor de comparação
    unsigned int _comparisons = 0;          // Número de comparações feitas

    void _clear(node<key_t, value_t>* n) {
        if (n == nullptr) {
            return;
        }
        _clear(n->left);
        _clear(n->right);
        delete n;
    }

    int _height(node<key_t, value_t>* n) const {
        if (n == nullptr) {
            return 0;
        }
        return n->height;
    }

    int _balance(node<key_t, value_t>* n) const {
        return _height(n->right) - _height(n->left);
    }

    node<key_t, value_t>* _right_rotation(node<key_t, value_t>* p) {
        node<key_t, value_t>* u = p->left;
        p->left = u->right;
        u->right = p;

        p->height = 1 + std::max(_height(p->left), _height(p->right));
        u->height = 1 + std::max(_height(u->left), _height(u->right));

        return u;
    }

    node<key_t, value_t>* _left_rotation(node<key_t, value_t>* p) {
        node<key_t, value_t>* u = p->right;
        p->right = u->left;
        u->left = p;

        p->height = 1 + std::max(_height(p->left), _height(p->right));
        u->height = 1 + std::max(_height(u->left), _height(u->right));

        return u;
    }

    node<key_t, value_t>* _insert(node<key_t, value_t>* p, key_t k, value_t v) {
        _comparisons++;
        if (p == nullptr) {
            _size++;
            return new node<key_t, value_t>(k, v);
        }

        _comparisons++;
        if (_compare(k, p->key.first)) {
            p->left = _insert(p->left, k, v);
        } else if (_compare(p->key.first, k)) {
            _comparisons++;
            p->right = _insert(p->right, k, v);
        } else {
            return p;
        }

        p = _fixup_node(p, k);
        return p;
    }

    node<key_t, value_t>* _fixup_node(node<key_t, value_t>* p, key_t k) {
        p->height = 1 + std::max(_height(p->left), _height(p->right));

        int bal = _balance(p);

        _comparisons++;
        if (bal < -1 && _compare(k, p->left->key.first)) {
            return _right_rotation(p);
        }

        _comparisons++;
        if (bal < -1 && _compare(p->left->key.first, k)) {
            p->left = _left_rotation(p->left);
            return _right_rotation(p);
        }

        _comparisons++;
        if (bal > 1 && _compare(p->right->key.first, k)) {
            return _left_rotation(p);
        }

        _comparisons++;
        if (bal > 1 && _compare(k, p->right->key.first)) {
            p->right = _right_rotation(p->right);
            return _left_rotation(p);
        }

        return p;
    }

    node<key_t, value_t>* _remove(node<key_t, value_t>* n, key_t k) {
        _comparisons++;
        if (n == nullptr) {
            return nullptr;
        }

        if (_compare(k, n->key.first)) {
            n->left = _remove(n->left, k);
        } else if (_compare(n->key.first, k)) {
            _comparisons++;
            n->right = _remove(n->right, k);
        } else if (n->right == nullptr) {
            _comparisons++;
            node<key_t, value_t>* temp = n->left;
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

    node<key_t, value_t>* _remove_successor(node<key_t, value_t>* n,
                                            node<key_t, value_t>* successor) {
        _comparisons++;
        if (successor->left != nullptr) {
            successor->left = _remove_successor(n, successor->left);
        } else {
            n->key = successor->key;
            node<key_t, value_t>* temp = successor->right;
            delete successor;
            return temp;
        }
        successor = _fixup_deletion(successor);
        return successor;
    }

    node<key_t, value_t>* _fixup_deletion(node<key_t, value_t>* n) {
        _comparisons++;
        if (n == nullptr) {
            return nullptr;
        }

        n->height = 1 + std::max(_height(n->left), _height(n->right));

        int bal = _balance(n);

        if (bal > 1 && _balance(n->right) >= 0) {
            return _left_rotation(n);
        }

        if (bal > 1 && _balance(n->right) < 0) {
            n->right = _right_rotation(n->right);
            return _left_rotation(n);
        }

        if (bal < -1 && _balance(n->left) <= 0) {
            return _right_rotation(n);
        }

        if (bal < -1 && _balance(n->left) > 0) {
            n->left = _left_rotation(n->left);
            return _right_rotation(n);
        }

        return n;
    }

    node<key_t, value_t>* _search(node<key_t, value_t>* n, key_t k) {
        _comparisons++;
        if (n == nullptr) {
            return nullptr;
        }

        _comparisons++;
        if (!_compare(k, n->key.first) && !_compare(n->key.first, k)) {
            return n;
        }

        _comparisons++;
        if (_compare(k, n->key.first)) {
            return _search(n->left, k);
        }
        return _search(n->right, k);
    }

   public:
    // Construtor da árvore
    avl_tree() : _compare(compare()) {}

    // Destruidor da árvore
    ~avl_tree() { _clear(_root); }

    // Insere uma chave e valor na árvore
    void insert(key_t k, value_t v) { _root = _insert(_root, k, v); }

    // Remove uma chave da árvore
    void remove(key_t k) { _root = _remove(_root, k); }

    // Limpa a árvore
    void clear() {
        _clear(_root);
        _root = nullptr;
        _size = 0;
    }

    // Retorna o número de nós na árvore
    unsigned int size() const { return _size; }

    // Retorna se a árvore está vazia
    bool empty() const { return _size == 0; }

    // Retorna a referência para o valor associado a uma chave
    value_t& search(key_t k) {
        node<key_t, value_t>* n = _search(_root, k);
        _comparisons++;
        if (n != nullptr) {
            return n->key.second;
        }

        throw std::out_of_range("Key not found");
    }

    // Verifica se uma chave está na árvore
    bool contains(key_t k) { return _search(_root, k) != nullptr; }

    // Atualiza o valor associado a uma chave
    void att(key_t k, value_t v) {
        node<key_t, value_t>* n = _search(_root, k);

        _comparisons++;
        if (n != nullptr) {
            n->key.second = v;
            return;
        }

        throw std::out_of_range("Key not found");
    }

    // Classe iterador
    class iterator {
       private:
        std::stack<node<key_t, value_t>*> _stack;
        node<key_t, value_t>* _current;

        void _push_left(node<key_t, value_t>* n) {
            while (n != nullptr) {
                _stack.push(n);
                n = n->left;
            }
        }

       public:
        iterator(node<key_t, value_t>* root) : _current(nullptr) {
            _push_left(root);
            if (!_stack.empty()) {
                _current = _stack.top();
                _stack.pop();
            }
        }

        iterator& operator++() {
            if (_current == nullptr) {
                return *this;
            }

            if (_current->right != nullptr) {
                _push_left(_current->right);
            }

            if (_stack.empty()) {
                _current = nullptr;
            } else {
                _current = _stack.top();
                _stack.pop();
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return _current != other._current;
        }

        const std::pair<key_t, value_t>& operator*() const {
            return _current->key;
        }

        const std::pair<key_t, value_t>* operator->() const {
            return &_current->key;
        }
    };

    iterator begin() { return iterator(_root); }

    iterator end() { return iterator(nullptr); }

    // Retorna o número de comparações realizadas
    unsigned int comparisons() const { return _comparisons; }
};
