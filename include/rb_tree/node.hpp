#pragma once

#include <iostream>

// Definições para as cores dos nós na árvore rubro-negra
#define RED false
#define BLACK true

// Estrutura que define um nó da árvore rubro-negra
template <typename type>
struct rb_node {
    type key;           // Chave do nó
    unsigned int freq;  // Frequência da chave
    bool color;         // Cor do nó (RED ou BLACK)
    rb_node *left;         // Ponteiro para o filho esquerdo
    rb_node *right;        // Ponteiro para o filho direito
    rb_node *parent;       // Ponteiro para o nó pai

    // Construtor para inicializar um nó com uma chave, cor, e ponteiros para o
    // filho esquerdo, direito e o pai
    rb_node(type key, bool color = RED, rb_node *left = nullptr, rb_node *right = nullptr,
         rb_node *parent = nullptr)
        : key(key), freq(1), color(color), left(left), right(right), parent(parent) {}
};
