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
template <typename key_t, typename value_t, typename hash = std::hash<key_t>,
          typename compare = std::less<key_t>>
class chained_hash_table {
   private:
    size_t _number_of_elements;  // Número de elementos na tabela hash
    size_t _table_size;          // Tamanho atual da tabela hash
    unsigned int _comparisons;   // Número de comparações realizadas

    // Vetor de listas de pares (chave, valor)
    std::vector<std::list<std::pair<key_t, value_t>>>* _table;

    // Fator de carga atual da tabela hash
    float _load_factor;

    // Fator de carga máximo permitido antes da rehashing
    float _max_load_factor;

    // Função de hash para gerar índices de tabela
    hash _hashing;

    // Functor de comparação
    compare _compare;

    // Vetor de chaves ordenadas (para iteradores)
    std::vector<std::pair<key_t, value_t>> _sorted_keys;

    // Flag para indicar se o vetor de chaves ordenadas está desatualizado
    bool _keys_dirty;

    // Encontra o próximo número primo maior ou igual a x
    size_t get_next_prime(size_t x) {
        // Se x for par, incrementa para o próximo ímpar
        _comparisons++;
        if (x % 2 == 0) {
            x++;
        }

        // Itera para encontrar o próximo número primo
        _comparisons++;
        while (true) {
            // Verifica se x é divisível por algum número ímpar até a raiz
            bool is_prime = true;
            _comparisons++;
            for (size_t i = 3; i <= std::sqrt(x); i += 2) {
                // Se x for divisível por i, não é um número primo
                _comparisons++;
                if (x % i == 0) {
                    is_prime = false;
                    break;
                }
                _comparisons++;
            }
            _comparisons++;

            // Se encontrou um número primo, retorna x
            _comparisons++;
            if (is_prime) {
                return x;
            }

            // Senão, vai para o próximo número ímpar
            x += 2;
        }
    }

    // Calcula o índice da tabela hash para uma chave k
    size_t hash_code(const key_t& k) const { return _hashing(k) % _table_size; }

    // Atualiza o vetor de chaves ordenadas
    void update_sorted_keys() {
        // Limpa o vetor de chaves ordenadas e reserva espaço
        _sorted_keys.clear();
        _sorted_keys.reserve(_number_of_elements);

        // Itera sobre cada bucket da tabela hash
        for (size_t i = 0; i < _table_size; i++) {
            // Adiciona cada par (chave, valor) ao vetor de chaves ordenadas
            for (const auto& p : (*_table)[i]) {
                _sorted_keys.push_back(p);
            }
        }

        // Ordena o vetor de chaves ordenadas usando o comparador
        std::sort(_sorted_keys.begin(), _sorted_keys.end(),
                  [this](const std::pair<key_t, value_t>& a,
                         const std::pair<key_t, value_t>& b) -> bool {
                      return _compare(a.first, b.first);
                  });

        _keys_dirty = false;
    }

   public:
    // Construtor padrão com tamanho inicial, função de hash e functor de
    // comparação
    chained_hash_table(size_t table_size = 19, const hash& hf = hash())
        : _number_of_elements(0),
          _table_size(get_next_prime(table_size)),
          _comparisons(0),
          _table(new std::vector<std::list<std::pair<key_t, value_t>>>(
              _table_size)),
          _load_factor(0),
          _max_load_factor(0.75),
          _hashing(hf),
          _compare(compare()),
          _sorted_keys(),
          _keys_dirty(true) {}

    // Destrutor da tabela hash
    ~chained_hash_table() {
        clear();
        delete _table;
    }

    // Retorna o número de slots (buckets) na tabela hash
    size_t bucket_count() const { return _table_size; }

    // Retorna o número de elementos em um slot (bucket) específico
    size_t bucket_size(size_t n) const { return (*_table)[n].size(); }

    // Retorna o índice do slot (bucket) de uma chave k
    size_t bucket(const key_t& k) const { return hash_code(k); }

    // Retorna o fator de carga atual
    float load_factor() const {
        return static_cast<float>(_number_of_elements) / _table_size;
    }

    // Retorna o fator de carga máximo
    float max_load_factor() const { return _max_load_factor; }

    // Define o fator de carga máximo
    void max_load_factor(float lf) { _max_load_factor = lf; }

    // Define o fator de carga atual e faz rehashing se necessário
    void load_factor(float lf) {
        // Verifica se o fator de carga está fora do intervalo
        _comparisons++;
        if (lf <= 0 || lf > _max_load_factor) {
            throw std::out_of_range("Out of range load factor");
        }

        _load_factor = lf;  // Atualiza o fator de carga

        // Redimensiona a tabela se o fator de carga exceder o máximo
        _comparisons++;
        if (load_factor() > _load_factor) {
            rehash(2 * _table_size);
        }
    }

    // Redimensiona a tabela hash para um novo tamanho
    void rehash(size_t new_size) {
        // Se o novo tamanho for menor ou igual ao atual, não faz nada
        _comparisons++;
        if (new_size <= _table_size) {
            return;
        }

        // Encontra o próximo número primo maior ou igual ao novo tamanho
        new_size = get_next_prime(new_size);

        // Cria uma nova tabela hash com o novo tamanho
        auto new_table =
            new std::vector<std::list<std::pair<key_t, value_t>>>(new_size);

        // Transfere os elementos da tabela antiga para a nova tabela
        _comparisons++;
        for (const auto& bucket : *_table) {
            _comparisons++;
            for (const auto& p : bucket) {
                size_t index = _hashing(p.first) % new_size;
                (*new_table)[index].push_back(p);
                _comparisons++;
            }
            _comparisons++;
        }
        _comparisons++;

        delete _table;           // Libera a memória da tabela antiga
        _table = new_table;      // Atualiza o ponteiro para a nova tabela
        _table_size = new_size;  // Atualiza o tamanho da tabela
        _keys_dirty = true;      // Marca as chaves como desatualizadas
    }

    // Insere a chave k na tabela hash associada ao valor v
    bool insert(const key_t k, const value_t v) {
        // Verifica se o fator de carga excedeu o máximo
        _comparisons++;
        if (load_factor() > _max_load_factor) {
            rehash(2 * _table_size);
        }

        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        _comparisons++;
        for (auto& p : (*_table)[i]) {
            _comparisons++;
            if (p.first == k) {
                return false;  // Chave já existe, não insere
            }
            _comparisons++;
        }
        _comparisons++;

        // Insere a chave k e o valor v na lista do slot correspondente
        (*_table)[i].push_back(std::make_pair(k, v));
        _number_of_elements++;  // Incrementa o número de elementos
        _keys_dirty = true;     // Marca as chaves como desatualizadas
        return true;            // Chave inserida com sucesso
    }

    // Remove a chave k da tabela hash
    bool remove(const key_t& k) {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        _comparisons++;
        for (auto it = (*_table)[i].begin(); it != (*_table)[i].end(); ++it) {
            _comparisons++;
            if (it->first == k) {
                (*_table)[i].erase(it);  // Remove a chave da lista
                _number_of_elements--;   // Decrementa o número de elementos
                _keys_dirty = true;      // Marca as chaves como desatualizadas
                return true;             // Chave removida com sucesso
            }
            _comparisons++;
        }
        _comparisons++;

        return false;  // Chave não encontrada, não remove
    }

    // Limpa todos os elementos da tabela hash
    void clear() {
        // Limpa cada bucket da tabela hash
        for (size_t i = 0; i < _table_size; i++) {
            (*_table)[i].clear();
        }

        // Reseta o número de elementos e a flag de chaves desatualizadas
        _number_of_elements = 0;
        _keys_dirty = true;
    }

    // Retorna o número de elementos na tabela hash
    size_t size() const { return _number_of_elements; }

    // Verifica se a tabela hash está vazia
    bool empty() const { return _number_of_elements == 0; }

    // Busca o valor associado à chave k na tabela hash
    value_t search(const key_t& k) {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        _comparisons++;
        for (auto& p : (*_table)[i]) {
            _comparisons++;
            if (p.first == k) {
                return p.second;
            }
            _comparisons++;
        }
        _comparisons++;

        // Se a chave não foi encontrada, lança uma exceção
        throw std::out_of_range("Key not found");
    }

    // Verifica se a chave k está na tabela hash
    bool contains(const key_t& k) const {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        for (const auto& p : (*_table)[i]) {
            if (p.first == k) {
                return true;  // Chave encontrada
            }
        }
        return false;  // Chave não encontrada
    }

    // Atualiza o valor associado à chave k
    void att(const key_t& k, value_t v) {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        _comparisons++;
        for (auto it = (*_table)[i].begin(); it != (*_table)[i].end(); ++it) {
            _comparisons++;
            if (it->first == k) {
                it->second = v;      // Atualiza o valor associado à chave k
                _keys_dirty = true;  // Marca as chaves como desatualizadas
                return;
            }
            _comparisons++;
        }
        _comparisons++;

        // Se a chave não foi encontrada, lança uma exceção
        throw std::out_of_range("Key not found");
    }

    // Sobrecarga do operador de indexação para acessar ou modificar o valor
    // associado à chave k
    value_t& operator[](const key_t& k) {
        // Calcula o índice da tabela hash para a chave k
        size_t i = hash_code(k);

        // Itera para encontrar a chave k na lista do slot correspondente
        _comparisons++;
        for (auto& p : (*_table)[i]) {
            _comparisons++;
            if (p.first == k) {
                return p.second;
            }
            _comparisons++;
        }
        _comparisons++;

        (*_table)[i].push_back(std::make_pair(k, value_t()));
        _number_of_elements++;  // Incrementa o número de elementos
        _keys_dirty = true;     // Marca as chaves como desatualizadas
        return (*_table)[i].back().second;
    }

    // Sobrecarga do operador de indexação constante para acessar o valor
    // associado à chave k
    const value_t& operator[](const key_t& k) const {
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

    // Classe de iterador para percorrer as chaves da tabela hash
    // O Iterador não acessa diretamente a tabela hash, mas sim um vetor de
    // chaves ordenadas
    class iterator {
       private:
        const chained_hash_table* _ht;  // Ponteiro para a tabela hash
        size_t _index;  // Índice atual do vetor de chaves ordenadas

       public:
        // Construtor do iterador
        iterator(const chained_hash_table* ht, size_t index)
            : _ht(ht), _index(index) {}

        // Sobrecarga do operador de desreferência para acessar o par (chave,
        // valor)
        const std::pair<key_t, value_t>& operator*() const {
            return _ht->_sorted_keys[_index];
        }

        // Sobrecarga do operador de incremento para avançar o iterador
        iterator& operator++() {
            _index++;
            return *this;
        }

        // Sobrecarga do operador de desigualdade para comparar dois iteradores
        bool operator!=(const iterator& it) const {
            return _index != it._index;
        }

        // Sobrecarga do operador de seta para acessar o ponteiro para o par
        const std::pair<key_t, value_t>* operator->() const {
            return &_ht->_sorted_keys[_index];
        }
    };

    // Retorna um iterador para o início do vetor de chaves ordenadas
    iterator begin() {
        // Se o vetor de chaves ordenadas estiver desatualizado, atualiza
        if (_keys_dirty) {
            update_sorted_keys();
        }

        // Cria um iterador para o início do vetor de chaves ordenadas
        return iterator(this, 0);
    }

    // Retorna um iterador para o final do vetor de chaves ordenadas
    iterator end() {
        // Se o vetor de chaves ordenadas estiver desatualizado, atualiza
        if (_keys_dirty) {
            update_sorted_keys();
        }

        // Cria um iterador para o final do vetor de chaves ordenadas
        return iterator(this, _sorted_keys.size());
    }

    // Retorna o número de comparações feitas
    unsigned int comparisons() const { return _comparisons; }
};
