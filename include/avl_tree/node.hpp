#pragma once

template <typename type>
struct node {
    type key;             // Chave do nó
    unsigned int freq;    // Frequência em que a chave aparece
    unsigned int height;  //  Altura do nó
    node* left;           // Ponteiro para o filho esquerdo
    node* right;          // Ponteiro para o filho direito

    // Construtor
    node(type key) {
        this->key = key;
        this->freq = 1;
        this->height = 1;
        this->left = nullptr;
        this->right = nullptr;
    }
};
