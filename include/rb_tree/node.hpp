#pragma once

#include <iostream>

#define RED false
#define BLACK true

template <typename key_t, typename value_t>
struct rb_node {
    std::pair<key_t, value_t> key;  // Par de chave e valor
    bool color;                     // Cor do nó (RED ou BLACK)
    rb_node* left;                  // Ponteiro para o filho esquerdo
    rb_node* right;                 // Ponteiro para o filho direito
    rb_node* parent;                // Ponteiro para o nó pai

    rb_node(const key_t& k, const value_t& v, bool c = RED,
            rb_node* l = nullptr, rb_node* r = nullptr, rb_node* p = nullptr)
        : key(std::make_pair(k, v)), color(c), left(l), right(r), parent(p) {}
};