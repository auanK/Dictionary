#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <vector>

// Função de hash para strings Unicode
struct hash_unicode {
    size_t operator()(const UnicodeString& s) const {
        size_t hash = 0;
        for (int i = 0; i < s.length(); i++) {
            hash = 31 * hash + s[i];
        }
        return hash;
    }
};

// Classe que implementa uma tabela hash com tratamento de colisão por
// encadeamento exterior
template <typename key, typename hash = std::hash<key>>
class hash_table {
   private:
    size_t _number_of_elements;  // Número de elementos na tabela hash
    size_t _table_size;          // Tamanho atual da tabela hash
    mutable unsigned int _comparisons = 0;  // Número de comparações realizadas

    // Vetor de listas para armazenar pares chave-valor
    std::vector<std::list<std::pair<key, int>>>* _table;

    // Fator de carga atual da tabela hash
    float _load_factor;

    // Fator de carga máximo permitido antes da rehashing
    float _max_load_factor;

    // Função de hash para gerar índices de tabela
    hash _hashing;

    // Encontra o próximo número primo maior ou igual a x
    size_t get_next_prime(size_t x) {
        // Se x for par, incrementa para o próximo número ímpar
        if (x % 2 == 0) {
            x++;
        }

        // Itera para encontrar o próximo número primo
        while (true) {
            // Verifica se x é divisível por algum número ímpar até a raiz
            bool is_prime = true;
            for (size_t i = 3; i <= std::sqrt(x); i += 2) {
                // Se x for divisível por i, não é um número primo
                if (x % i == 0) {
                    is_prime = false;
                    break;
                }
            }
            // Se encontrou um número primo, retorna x
            if (is_prime) {
                return x;
            }
            // Senão, vai para o próximo número ímpar
            x += 2;
        }
    }

    // Calcula o índice da tabela hash para uma chave k
    size_t hash_code(const key& k) const { return _hashing(k) % _table_size; }

   public:
    // Construtor padrão da tabela hash com tamanho inicial e função de hash
    hash_table(size_t table_size = 19, const hash& hf = hash())
        : _number_of_elements(0),
          _table_size(table_size),
          _table(new std::vector<std::list<std::pair<key, int>>>(table_size)),
          _load_factor(0.0),
          _max_load_factor(0.75),
          _hashing(hf),
          _comparisons(0) {}

    // Construtor de cópia
    hash_table(const hash_table& t)
        : _number_of_elements(t._number_of_elements),
          _table_size(t._table_size),
          _table(new std::vector<std::list<std::pair<key, int>>>(*t._table)),
          _load_factor(t._load_factor),
          _max_load_factor(t._max_load_factor),
          _hashing(t._hashing),
          _comparisons(t._comparisons) {}

    // Sobrecarga do operador de atribuição
    hash_table& operator=(const hash_table& t) {
        if (this == &t) return *this;  // Verifica auto-atribuição

        // Libera a memória da tabela antiga
        delete _table;

        // Copia o número de elementos e o tamanho da tabela
        _number_of_elements = t._number_of_elements;
        _table_size = t._table_size;

        // Cria uma nova tabela e copia os elementos
        _table = new std::vector<std::list<std::pair<key, int>>>(*t._table);

        _load_factor = t._load_factor;          // Copia o fator de carga atual
        _max_load_factor = t._max_load_factor;  // Copia o fator de carga máximo
        _hashing = t._hashing;                  // Copia a função de hash

        return *this;
    }

    // Destrutor da tabela hash
    ~hash_table() {
        clear();
        delete _table;
    }

    // Retorna o número de elementos na tabela hash
    size_t size() const { return _number_of_elements; }

    // Retorna true se a tabela hash estiver vazia
    bool empty() const { return _number_of_elements == 0; }

    // Retorna o número de slots (buckets) na tabela hash
    size_t bucket_count() const { return _table_size; }

    // Retorna o número de elementos no slot (bucket) n da tabela hash
    size_t bucket_size(size_t n) const { return (*_table)[n].size(); }

    // Retorna o slot (bucket) da tabela hash onde a chave k deve ser
    // armazenada
    size_t bucket(const key& k) const { return hash_code(k); }

    // Limpa todos os elementos da tabela hash
    void clear() {
        // Limpa todas as listas de pares chave-valor na tabela
        for (size_t i = 0; i < _table_size; i++) {
            (*_table)[i].clear();
        }

        // Reseta o número de elementos
        _number_of_elements = 0;
    }

    // Calcula o fator de carga atual da tabela hash
    float load_factor() const {
        return static_cast<float>(_number_of_elements) / _table_size;
    }

    // Retorna o fator de carga máximo da tabela hash
    float max_load_factor() const { return _max_load_factor; }

    // Define o fator de carga máximo da tabela hash
    void max_load_factor(float lf) { _max_load_factor = lf; }

    // Define o fator de carga atual e redimensiona a tabela se necessário
    void load_factor(float lf) {
        // Verifica se o fator de carga está fora do intervalo permitido
        if (lf <= 0 || lf > _max_load_factor) {
            throw std::out_of_range("Out of range load factor");
        }

        _load_factor = lf;  // Atualiza o fator de carga

        // Redimensiona a tabela se o fator de carga exceder o máximo
        if (load_factor() > _load_factor) {
            rehash(2 * _table_size);
        }
    }

    // Insere a chave k com um valor associado na tabela hash
    bool insert(const key& k) {
        // Verifica se o fator de carga excede o máximo permitido
        if (load_factor() > _max_load_factor) {
            rehash(2 * _table_size);
        }

        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        for (auto& p : (*_table)[i]) {
            if (p.first == k) {
                p.second++;
                return false;
            }
        }

        // Adiciona a chave e o valor no slot correspondente
        (*_table)[hash_code(k)].push_back(std::make_pair(k, 1));
        _number_of_elements++;
        return true;  // A chave foi adicionada, retorna true
    }

    // Verifica se a chave k está na tabela hash
    bool contains(const key& k) const {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        for (const auto& p : (*_table)[i]) {
            if (p.first == k) {
                return true;  // A chave foi encontrada
            }
        }

        return false;  // A chave não foi encontrada
    }

    // Remove a chave k da tabela hash
    bool remove(const key& k) {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        for (auto it = (*_table)[i].begin(); it != (*_table)[i].end(); ++it) {
            if (it->first == k) {
                (*_table)[i].erase(it);  // Remove a chave do slot
                _number_of_elements--;   // Decrementa o número de elementos
                return true;             // A chave foi removida
            }
        }

        return false;  // A chave não foi encontrada
    }

    // Retorna o valor associado à chave k
    int& at(const key& k) {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        for (auto& p : (*_table)[i]) {
            if (p.first == k) {
                return p.second;
            }
        }

        // Se a chave não foi encontrada, lança uma exceção
        throw std::out_of_range("Key not found");
    }

    // Retorna o valor associado à chave k (Versão const)
    const int& at(const key& k) const {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        for (const auto& p : (*_table)[i]) {
            if (p.first == k) {
                return p.second;
            }
        }

        // Se a chave não foi encontrada, lança uma exceção
        throw std::out_of_range("Key not found");
    }

    // Sobrecarga do operador de indexação para acessar ou modificar o valor da
    // chave k
    int& operator[](const key& k) {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        for (auto& p : (*_table)[i]) {
            if (p.first == k) {
                return p.second;
            }
        }

        // Adiciona a chave e o valor no slot correspondente
        (*_table)[i].push_back(std::make_pair(k, 0));
        _number_of_elements++;
        return (*_table)[i].back().second;
    }

    // Sobrecarga do operador de indexação para acessar o valor associado à
    // chave k (Versão const)
    const int& operator[](const key& k) const { return at(k); }

    // Redimensiona a tabela hash para um novo tamanho
    void rehash(size_t new_size) {
        // Se o novo tamanho for menor ou igual ao tamanho atual, não faz nada
        if (new_size <= _table_size) {
            return;
        }

        // Encontra o próximo número primo maior ou igual ao novo tamanho
        new_size = get_next_prime(new_size);

        // Cria uma nova tabela com o novo tamanho
        auto new_table =
            new std::vector<std::list<std::pair<key, int>>>(new_size);

        // Transfere os elementos da tabela antiga para a nova tabela
        for (const auto& bucket : *_table) {
            for (const auto& p : bucket) {
                size_t index = _hashing(p.first) % new_size;
                (*new_table)[index].push_back(p);
            }
        }

        delete _table;           // Libera a memória da tabela antiga
        _table = new_table;      // Atualiza o ponteiro para a nova tabela
        _table_size = new_size;  // Atualiza o tamanho da tabela
    }
    // Classe iterador para a tabela hash
    class iterator {
       private:
        size_t _index;             // Índice atual do slot na tabela
        const hash_table* _table;  // Ponteiro para a tabela hash

        // Iterador para a lista de pares chave-valor no slot atual
        typename std::list<std::pair<key, int>>::iterator _it;

        void advance_to_next_nonempty() {
            // Avança para o próximo slot não vazio
            while (_index < _table->_table_size &&
                   (*_table->_table)[_index].empty()) {
                ++_index;
            }

            // Se o índice for menor que o tamanho da tabela, obtém o iterador
            // se não, cria um iterador vazio
            if (_index < _table->_table_size) {
                _it = (*_table->_table)[_index].begin();
            } else {
                _it = typename std::list<std::pair<key, int>>::iterator();
            }
        }

       public:
        // Construtor do iterador
        iterator(size_t index, const hash_table* table)
            : _index(index), _table(table) {
            if (_index < _table->_table_size) {
                _it = (*_table->_table)[_index].begin();
                advance_to_next_nonempty();
            }
        }

        // Método para buscar um elemento no iterador
        iterator find(const key& k) {
            size_t i = _table->hash_code(k);  // Calcula o índice
            auto& bucket =
                (*_table->_table)[i];  // Obtém a lista correspondente

            // Procura o elemento na lista
            for (auto it = bucket.begin(); it != bucket.end(); ++it) {
                if (it->first == k) {
                    return iterator(
                        i, _table,
                        it);  // Retorna o iterador para o elemento encontrado
                }
            }
            return end();  // Se não encontrar, retorna o iterador para o final
        }

        // Sobrecarga do operador de pré-incremento (++it)
        iterator& operator++() {
            if (_index >= _table->_table_size) {
                return *this;
            }

            ++_it;

            if (_it == (*_table->_table)[_index].end()) {
                ++_index;
                advance_to_next_nonempty();
            }
            return *this;
        }

        // Sobrecarga do operador de diferença (it1 != it2)
        bool operator!=(const iterator& other) const {
            return _index != other._index || _it != other._it;
        }

        // Sobrecarga do operador de indireção (*it)
        const std::pair<key, int>& operator*() const { return *_it; }

        // Sobrecarga do operador de seta (it->first)
        const std::pair<key, int>* operator->() const { return &(*_it); }
    };

    // Retorna um iterador para o início da tabela hash
    iterator begin() const { return iterator(0, this); }

    // Retorna um iterador para o final da tabela hash
    iterator end() const { return iterator(_table_size, this); }

    // Retorna o número de comparações realizadas
    unsigned int comparisons() const { return _comparisons; }
};