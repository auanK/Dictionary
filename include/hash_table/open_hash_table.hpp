#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// Classe que implementa uma tabela hash com tratamento de colisão por
// endereçamento aberto (hashing duplo)
template <typename key_t, typename value_t, typename hash = std::hash<key_t>,
          typename compare = std::less<key_t>>
class open_hash_table {
   private:
    // Enumeração para os estados de uma entrada na tabela hash
    enum entry_state { EMPTY, OCCUPIED, DELETED };

    // Estrutura de uma entrada na tabela hash
    struct entry {
        key_t key{};               // Chave da entrada
        value_t value{};           // Valor associado à chave
        entry_state state{EMPTY};  // Estado da entrada
    };

    size_t _number_of_elements;  // Número de elementos na tabela hash
    size_t _table_size;          // Tamanho atual da tabela hash
    std::vector<entry> _table;   // Vetor de entradas da tabela hash
    float _load_factor;          // Fator de carga atual da tabela hash
    float _max_load_factor;      // Fator de carga máximo permitido
    hash _hash;        // Função de hash para gerar índices de tabela
    compare _compare;  // Functor de comparação

    // Vetor de chaves ordenadas (para iteradores)
    std::vector<std::pair<key_t, value_t>> _sorted_keys;

    // Flag para indicar se o vetor de chaves ordenadas está desatualizado
    bool _keys_dirty;

    unsigned int _comparisons;  // Número de comparações realizadas

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

    // Calcula o índice da tabela hash para uma chave k (hashing duplo)
    size_t hash_code(const key_t &k, const size_t i = 0) const {
        size_t hash1 = _hash(k);
        size_t hash2 = 1 + (hash1 % (_table_size - 1));
        return (hash1 + i * hash2) % _table_size;
    }

    // Atualiza o vetor de chaves ordenadas
    void update_sorted_keys() {
        // Limpa o vetor de chaves ordenadas
        _sorted_keys.clear();
        _sorted_keys.reserve(_number_of_elements);

        // Itera sobre cada entrada da tabela hash
        for (const auto &entry : _table) {
            // Se a entrada está ocupada, adiciona a chave ao vetor de chaves
            if (entry.state == OCCUPIED) {
                _sorted_keys.emplace_back(entry.key, entry.value);
            }
        }

        // Ordena o vetor de chaves usando o comparador
        std::sort(_sorted_keys.begin(), _sorted_keys.end(),
                  [this](const std::pair<key_t, value_t> &a,
                         const std::pair<key_t, value_t> &b) -> bool {
                      return _compare(a.first, b.first);
                  });

        _keys_dirty = false;  // Marca as chaves como atualizadas
    }

   public:
    // Construtor padrão da tabela hash
    open_hash_table(size_t table_size = 19, const hash &hf = hash())
        : _number_of_elements(0),
          _table_size(get_next_prime(table_size)),
          _table(_table_size),
          _load_factor(0),
          _max_load_factor(0.75),
          _hash(hf),
          _compare(compare()),
          _sorted_keys(),
          _keys_dirty(true),
          _comparisons(0) {}

    // Destrutor da tabela hash
    ~open_hash_table() { clear(); }

    // Retorna o número de slots(buckets) na tabela hash
    size_t bucket_count() const { return _table_size; }

    // Retorna o fator de carga atual da tabela hash
    float load_factor() const {
        return static_cast<float>(_number_of_elements) / _table_size;
    }

    // Retorna o fator de carga máximo permitido
    float max_load_factor() const { return _max_load_factor; }

    // Define o fator de carga da tabela hash
    void load_factor(float lf) {
        // Verifica se o fator de carga está fora do intervalo permitido
        _comparisons++;
        if (lf <= 0 || lf > _max_load_factor) {
            throw std::out_of_range("load factor out of range");
        }

        _load_factor = lf;  // Atualiza o fator de carga

        // Redimensiona a tabela se o fator de carga exceder o máximo
        _comparisons++;
        if (load_factor() > _load_factor) {
            rehash(2 * _table_size);
        }
    }

    // Define o fator de carga máximo permitido
    void max_load_factor(float lf) {
        // Verifica se o fator de carga máximo está fora do intervalo permitido
        _comparisons++;
        if (lf <= 0 || lf < _load_factor) {
            throw std::out_of_range("max load factor out of range");
        }
        _max_load_factor = lf;  // Atualiza o fator de carga máximo
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
        std::vector<entry> new_table(new_size);

        // Transfere as entradas da tabela antiga para a nova tabela
        _comparisons++;
        for (size_t i = 0; i < _table_size; i++) {
            _comparisons++;
            if (_table[i].state == OCCUPIED) {
                size_t j = 0;
                size_t index;
                do {
                    index =
                        (_hash(_table[i].key) +
                         j * (1 + (_hash(_table[i].key) % (new_size - 1)))) %
                        new_size;
                    j++;
                    _comparisons++;
                } while (new_table[index].state == OCCUPIED);

                new_table[index].key = _table[i].key;
                new_table[index].value = _table[i].value;
                new_table[index].state = OCCUPIED;
            }
        }
        _comparisons++;

        _table_size = new_size;  // Atualiza o tamanho da tabela
        _table =
            std::move(new_table);  // Atualiza o ponteiro para a nova tabela
        _keys_dirty = true;        // Marca as chaves como desatualizadas
    }

    // Insere a chave k na tabela hash associada ao valor v
    bool insert(const key_t &k, const value_t &v) {
        // Verifica se o fator de carga excedeu o máximo
        _comparisons++;
        if (load_factor() > _max_load_factor) {
            rehash(2 * _table_size);
        }

        // Itera para encontrar um slot vazio ou a chave k
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);  // Calcula o índice da tabela hash
            // Se o slot está vazio, insere a chave e o valor
            _comparisons++;
            if (_table[index].state != OCCUPIED) {
                _table[index].key = k;
                _table[index].value = v;
                _table[index].state = OCCUPIED;
                _number_of_elements++;
                _keys_dirty = true;
                return true;  // Chave inserida com sucesso
            } else if (_table[index].key == k) {
                _comparisons++;
                return false;  // Chave já existe, não insere
            }

            _comparisons++;
        } while (i < _table_size);

        return false;  // Tabela cheia, não insere
    }

    // Remove a chave k da tabela hash
    bool remove(const key_t &k) {
        // Itera para encontrar a chave k na tabela hash
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);  // Calcula o índice da tabela hash
            _comparisons++;
            if (_table[index].state == EMPTY) {
                return false;  // Chave não encontrada
            }
            // Se a chave foi encontrada, remove a entrada
            else if (_table[index].state == OCCUPIED &&
                     _table[index].key == k) {
                _comparisons++;
                _table[index].state = DELETED;
                _number_of_elements--;
                _keys_dirty = true;
                return true;  // Chave removida com sucesso
            }
            _comparisons++;
        } while (i < _table_size);

        return false;  // Chave não encontrada
    }

    // Limpa todos os elementos da tabela hash
    void clear() {
        _table.clear();              // Limpa o vetor de entradas
        _table.resize(_table_size);  // Redimensiona o vetor
        _number_of_elements = 0;     // Reseta o número de elementos
        _keys_dirty = true;          // Marca as chaves como desatualizadas
    }

    // Retorna o número de elementos na tabela hash
    size_t size() const { return _number_of_elements; }

    // Verifica se a tabela hash está vazia
    bool empty() const { return _number_of_elements == 0; }

    // Busca o valor associado à chave k na tabela hash
    value_t search(const key_t &k) {
        // Itera para encontrar a chave k na tabela hash
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);  // Calcula o índice da tabela hash
            // Se o slot está vazio, a chave não existe
            _comparisons++;
            if (_table[index].state == EMPTY) {
                break;
            }
            // Se a chave foi encontrada, retorna o valor associado
            else if (_table[index].state == OCCUPIED &&
                     _table[index].key == k) {
                _comparisons++;
                return _table[index].value;
            }

            _comparisons++;
        } while (i < _table_size);

        // Se a chave não foi encontrada, lança uma exceção
        throw std::out_of_range("Key not found");
    }

    // Verifica se a chave k está na tabela hash
    bool contains(const key_t &k) const {
        // Itera para encontrar a chave k na tabela hash
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);  // Calcula o índice da tabela hash
            // Se o slot está vazio, a chave não existe
            if (_table[index].state == EMPTY) {
                return false;
            }
            // Se a chave foi encontrada, retorna verdadeiro
            else if (_table[index].state == OCCUPIED &&
                     _table[index].key == k) {
                return true;
            }
        } while (i < _table_size);

        return false;  // Chave não encontrada
    }

    void att(const key_t &k, const value_t &v) {
        // Itera para encontrar a chave k na tabela hash
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);  // Calcula o índice da tabela hash
            // A chave foi encontrada, atualiza o valor associado
            _comparisons++;
            if (_table[index].state == OCCUPIED && _table[index].key == k) {
                _table[index].value = v;
                return;
            }
            _comparisons++;
        } while (i < _table_size);

        // Se a chave não foi encontrada, lança uma exceção
        throw std::out_of_range("Key not found");
    }

    // Classe de iterador para percorrer as chaves da tabela hash
    // O Iterador não acessa diretamente a tabela hash, mas sim um vetor de
    // chaves ordenadas
    class iterator {
       private:
        const open_hash_table *_ht;  // Ponteiro para a tabela hash
        size_t _index;  // Índice atual do vetor de chaves ordenadas

       public:
        // Construtor do iterador
        iterator(const open_hash_table *ht, size_t index)
            : _ht(ht), _index(index) {}

        // Sobrecarga do operador de desreferência para acessar o par (chave,
        // valor)
        const std::pair<key_t, value_t> &operator*() const {
            return _ht->_sorted_keys[_index];
        }

        // Sobrecarga do operador de incremento para avançar o iterador
        iterator &operator++() {
            _index++;
            return *this;
        }

        // Sobrecarga do operador de desigualdade para comparar dois iteradores
        bool operator!=(const iterator &other) const {
            return _index != other._index;
        }

        // Sobrecarga do operador de seta para acessar o ponteiro para o par
        std::pair<key_t, value_t> *operator->() const {
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

    // Retorna o número de comparações realizadas
    unsigned int comparisons() const { return _comparisons; }
};
