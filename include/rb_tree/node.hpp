#pragma once

#include <iostream>

// Definições para as cores dos nós na árvore rubro-negra
#define RED false
#define BLACK true

// Estrutura que define um nó da árvore rubro-negra
template <typename type>
struct node {
    type key;           // Chave do nó
    unsigned int freq;  // Frequência da chave
    bool color;         // Cor do nó (RED ou BLACK)
    node *left;         // Ponteiro para o filho esquerdo
    node *right;        // Ponteiro para o filho direito
    node *parent;       // Ponteiro para o nó pai

    // Construtor para inicializar um nó com uma chave, cor, e ponteiros para o
    // filho esquerdo, direito e o pai
    node(type key, bool color = RED, node *left = nullptr, node *right = nullptr,
         node *parent = nullptr)
        : key(key), freq(1), color(color), left(left), right(right), parent(parent) {}
};
