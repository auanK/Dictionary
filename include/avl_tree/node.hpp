#pragma once
#include <utility>  

template <typename key_t, typename value_t>
struct node {
    std::pair<key_t, value_t> key;  
    node* left;
    node* right;
    int height;

    node(key_t k, value_t v)
        : key(std::make_pair(k, v)), left(nullptr), right(nullptr), height(1) {}
};
