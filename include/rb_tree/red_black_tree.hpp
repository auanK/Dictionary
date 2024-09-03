#pragma once

#include <iostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#define RED false
#define BLACK true

#include "node.hpp"

template <typename key_t, typename value_t, typename compare = std::less<key_t>>
class red_black_tree {
   private:
    rb_node<key_t, value_t>* _root;  // Raiz da árvore
    rb_node<key_t, value_t>* _nil;   // Nó sentinela que representa nulo (nil)
    unsigned int _size = 0;          // Número de elementos na árvore
    compare _compare;                // Functor de comparação
    unsigned int _comparisons = 0;   // Número de comparações feitas

    void _clear(rb_node<key_t, value_t>* n) {
        if (n != _nil) {
            _clear(n->left);
            _clear(n->right);
            delete n;
        }
    }

    void _left_rotate(rb_node<key_t, value_t>* x) {
        // Salva o nó y (filho direito de x)
        rb_node<key_t, value_t>* y = x->right;

        _comparisons++;
        x->right = y->left;  // Atualiza o pai do filho esquerdo de y
        if (y->left != _nil) {
            y->left->parent = x;  // Atualiza o pai do filho esquerdo de y
        }

        _comparisons++;
        y->parent = x->parent;  // Atualiza o pai de y para o pai de x
        if (x->parent == _nil) {
            _root = y;  // Se x era a raiz, y se torna a nova raiz
        } else if (x == x->parent->left) {
            _comparisons++;
            x->parent->left = y;  // Atualiza o filho esquerdo do pai de x
        } else {
            _comparisons++;
            x->parent->right = y;  // Atualiza o filho direito do pai de x
        }

        // Atualiza os filhos de y
        y->left = x;
        x->parent = y;
    }

    void _right_rotate(rb_node<key_t, value_t>* x) {
        // Salva o nó y (filho esquerdo de x)
        rb_node<key_t, value_t>* y = x->left;

        _comparisons++;
        x->left = y->right;  // Atualiza o pai do filho direito de y
        if (y->right != _nil) {
            y->right->parent = x;  // Atualiza o pai do filho direito de y
        }

        _comparisons++;
        y->parent = x->parent;  // Atualiza o pai de y para o pai de x
        if (x->parent == _nil) {
            _root = y;  // Se x era a raiz, y se torna a nova raiz
        } else if (x == x->parent->left) {
            _comparisons++;
            x->parent->left = y;  // Atualiza o filho esquerdo do pai de x
        } else {
            _comparisons++;
            x->parent->right = y;  // Atualiza o filho direito do pai de x
        }

        // Atualiza os filhos de y
        y->right = x;
        x->parent = y;
    }

    // Corrige a árvore após a inserção de um nó para manter as propriedades da
    // árvore rubro-negra
    void _insert_fixup(rb_node<key_t, value_t>* z) {
        _comparisons++;
        while (z->parent->color == RED) {
            _comparisons++;
            if (z->parent == z->parent->parent->left) {
                rb_node<key_t, value_t>* y =
                    z->parent->parent->right;  // Tio de z
                _comparisons++;
                if (y->color == RED) {
                    // Caso 1: O tio de z é vermelho
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;  // Move z para o avô
                } else {
                    _comparisons++;
                    if (z == z->parent->right) {
                        // Caso 2: z é filho direito
                        z = z->parent;
                        _left_rotate(z);  // Rotaciona á esquerda
                    }
                    // Caso 3: z é filho esquerdo
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    _right_rotate(z->parent->parent);
                }
            } else {
                rb_node<key_t, value_t>* y =
                    z->parent->parent->left;  // Tio de z
                _comparisons++;
                if (y->color == RED) {
                    // Caso 1: O tio de z é vermelho
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;  // Move z para o avô
                } else {
                    _comparisons++;
                    if (z == z->parent->left) {
                        // Caso 2: z é filho esquerdo
                        z = z->parent;
                        _right_rotate(z);  // Rotaciona á direita
                    }
                    // Caso 3: z é filho direito
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    _left_rotate(z->parent->parent);
                }
            }
            _comparisons++;
        }
        _comparisons++;
        _root->color = BLACK;  // Garante que a raiz é preta
    }

    // Remove o nó z da árvore e ajusta a árvore para manter suas propriedades
    void _remove(rb_node<key_t, value_t>* z) {
        rb_node<key_t, value_t>*y, *x;
        _comparisons++;
        if (z->left == _nil || z->right == _nil) {
            y = z;  // Caso em que z tem no máximo um filho
        } else {
            y = _minimum(z->right);  // Caso em que z tem dois filhos
        }

        _comparisons++;
        // Define x como o filho de y
        if (y->left != _nil) {
            x = y->left;
        } else {
            x = y->right;
        }

        // Atualiza o pai de x
        x->parent = y->parent;
        _comparisons++;
        if (y->parent == _nil) {
            _root = x;  // Caso em que y é a raiz
        } else if (y == y->parent->left) {
            _comparisons++;
            y->parent->left = x;  // Atualiza o filho esquerdo do pai de y
        } else {
            _comparisons++;
            y->parent->right = x;  // Atualiza o filho direito do pai de y
        }

        _comparisons++;
        // Copia o valor de y para z se necessário
        if (y != z) {
            z->key = y->key;
        }

        _comparisons++;
        // Se a cor de y é preta, ajusta a árvore
        if (y->color == BLACK) {
            _remove_fixup(x);
        }

        delete y;  // Libera a memória do nó removido
    }

    // Corrige a árvore após a remoção de um nó para manter as propriedades da
    // árvore rubro-negra
    void _remove_fixup(rb_node<key_t, value_t>* x) {
        while (x != _root && x->color == BLACK) {
            _comparisons++;
            if (x == x->parent->left) {
                rb_node<key_t, value_t>* w = x->parent->right;  // Irmão de x
                _comparisons++;
                if (w->color == RED) {
                    // Caso 1: O irmão de x é vermelho
                    w->color = BLACK;
                    x->parent->color = RED;
                    _left_rotate(x->parent);  // Rotaciona á esquerda
                    w = x->parent->right;     // Atualiza o irmão de x
                }
                _comparisons += 2;
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    // Caso 2: Ambos os filhos de w são pretos
                    w->color = RED;  // Torna w vermelho
                    x = x->parent;   // Move x para o pai
                } else {
                    _comparisons++;
                    if (w->right->color == BLACK) {
                        // Caso 3: O filho direito de w é preto
                        w->left->color = BLACK;
                        w->color = RED;
                        _right_rotate(w);      // Rotaciona w para a direita
                        w = x->parent->right;  // Atualliza w
                    }
                    // Caso 4: O filho direito de w é vermelho
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    _left_rotate(x->parent);  // Rotaciona á esquerda
                    x = _root;                // Move x para a raiz
                }
            } else {
                rb_node<key_t, value_t>* w = x->parent->left;  // Irmão de x
                _comparisons++;
                if (w->color == RED) {
                    // Caso 1: O irmão de x é vermelho
                    w->color = BLACK;
                    x->parent->color = RED;
                    _right_rotate(x->parent);  // Rotaciona á direita
                    w = x->parent->left;       // Atualiza w
                }
                _comparisons += 2;
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    // Caso 2: Ambos os filhos de w são pretos
                    w->color = RED;  // Torna w vermelho
                    x = x->parent;   // Move x para o pai
                } else {
                    _comparisons++;
                    if (w->left->color == BLACK) {
                        // Caso 3: O filho esquerdo de w é preto
                        w->right->color = BLACK;
                        w->color = RED;
                        _left_rotate(w);      // Rotaciona w para a esquerda
                        w = x->parent->left;  // Atualiza w
                    }
                    // Caso 4: O filho esquerdo de w é vermelho
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    _right_rotate(x->parent);  // Rotaciona á direita
                    x = _root;                 // Move x para a raiz
                }
            }
        }
        _comparisons++;
        x->color = BLACK;  // Garante que a raiz é preta
    }

    // Encontra o nó com a menor chave na subárvore com raiz em x
    rb_node<key_t, value_t>* _minimum(rb_node<key_t, value_t>* x) {
        _comparisons++;
        while (x->left != _nil) {
            x = x->left;  // Desce para o filho esquerdo
            _comparisons++;
        }
        _comparisons++;
        return x;  // x é o nó com a menor chave
    }

    // Encontra o nó com a maior chave na subárvore com raiz em x
    rb_node<key_t, value_t>* _maximum(rb_node<key_t, value_t>* x) {
        _comparisons++;
        while (x->right != _nil) {
            x = x->right;  // Desce para o filho direito
            _comparisons++;
        }
        _comparisons++;
        return x;  // x é o nó com a maior chave
    }

    rb_node<key_t, value_t>* _search(rb_node<key_t, value_t>* n,
                                     const key_t& k) {
        _comparisons++;
        while (n != _nil && k != n->key.first) {
            _comparisons += 2;
            if (_compare(k, n->key.first)) {
                n = n->left;
            } else {
                n = n->right;
            }
        }
        _comparisons++;
        return n;
    }

   public:
    // Contrutor que inicializa a árvore com um nó sentinela _nil
    red_black_tree()
        : _root(nullptr),
          _nil(new rb_node<key_t, value_t>(key_t(), value_t(), BLACK)) {
        _root = _nil;
    }

    // Destrutor que libera a memória alocada para os nós da árvore
    ~red_black_tree() {
        _clear(_root);
        delete _nil;
    }

    // Insere um novo valor na árvore
    void insert(const key_t& k, const value_t& v) {
        rb_node<key_t, value_t>* current = _root;
        rb_node<key_t, value_t>* current_parent = _nil;

        // Encontra o local correto para inserir o novo nó'
        _comparisons++;
        while (current != _nil) {
            current_parent = current;
            _comparisons++;
            if (_compare(k, current->key.first)) {
                current = current->left;
            } else if (_compare(current->key.first, k)) {
                _comparisons++;
                current = current->right;
            } else {
                _comparisons++;
                return;
            }
            _comparisons++;
        }

        // Cria o novo nó e define seu pai
        rb_node<key_t, value_t>* new_node =
            new rb_node<key_t, value_t>(k, v, RED, _nil, _nil, current_parent);

        _comparisons++;
        if (current_parent == _nil) {
            _root = new_node;  // A árvore estava vazia
        } else if (_compare(k, current_parent->key.first)) {
            _comparisons++;
            current_parent->left = new_node;  // Insere como filho esquerdo
        } else {
            _comparisons++;
            current_parent->right = new_node;  // Insere como filho direito
        }

        _size++;  // Incrementa o tamanho da árvore

        // Corrige a árvore para manter as propriedades da árvore rubro-negra
        _insert_fixup(new_node);
    }

    // Remove um valor da árvore
    void remove(const key_t& k) {
        rb_node<key_t, value_t>* p = _root;
        _comparisons++;
        while (p != _nil && k != p->key.first) {
            _comparisons += 2;
            if (_compare(k, p->key.first)) {
                p = p->left;
            } else {
                p = p->right;
            }
        }

        _comparisons++;
        if (p != _nil) {
            _remove(p);  // Remove o nó encontrado
            _size--;     // Decrementa o tamanho da árvore
        }
    }

    // Limpa a árvore
    void clear() {
        _clear(_root);
        _root = _nil;
        _size = 0;
    }

    // Retorna o tamanho da árvore
    unsigned int size() const { return _size; }

    // Verifica se a árvore está vazia
    bool empty() const { return _size == 0; }

    // Retorna o valor associado à chave k
    value_t search(const key_t& k) {
        rb_node<key_t, value_t>* node = _search(_root, k);
        _comparisons++;
        if (node != _nil) {
            return node->key.second;
        }

        throw std::out_of_range("Key not found");
    }

    // Verifica se a árvore contém a chave k
    bool contains(const key_t& k) { return _search(_root, k) != _nil; }

    // Atualiza o valor associado à chave k
    void att(const key_t& k, const value_t& v) {
        rb_node<key_t, value_t>* n = _search(_root, k);
        _comparisons++;
        if (n == _nil) {
            return;
        }

        n->key.second = v;
        return;
    }

    // Iterador para a árvore rubro-negra
    class iterator {
       private:
        rb_node<key_t, value_t>* _current;
        rb_node<key_t, value_t>* _nil;

       public:
        iterator(rb_node<key_t, value_t>* root, rb_node<key_t, value_t>* nil)
            : _nil(nil), _current(root) {}

        std::pair<key_t, value_t>& operator*() const { return _current->key; }

        std::pair<key_t, value_t>* operator->() const {
            return &(_current->key);
        }

        iterator& operator++() {
            if (_current == _nil) {
                return *this;
            }

            if (_current->right != _nil) {
                _current = _current->right;
                while (_current->left != _nil) {
                    _current = _current->left;
                }
            } else {
                rb_node<key_t, value_t>* y = _current->parent;
                while (y != _nil && _current == y->right) {
                    _current = y;
                    y = y->parent;
                }
                _current = y;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return _current != other._current;
        }
    };

    // Retorna um iterador para o início da árvore
    iterator begin() {
        rb_node<key_t, value_t>* n = _root;
        while (n->left != _nil) {
            n = n->left;
        }
        return iterator(n, _nil);
    }

    // Retorna um iterador para o final da árvore
    iterator end() { return iterator(_nil, _nil); }

    // Retorna o número de comparações feitas
    unsigned int comparisons() const { return _comparisons; }
};
