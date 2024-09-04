#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename key_t, typename value_t, typename hash = std::hash<key_t>,
          typename compare = std::less<key_t>>
class open_hash_table {
   private:
    enum entry_state { EMPTY, OCCUPIED, DELETED };

    struct entry {
        key_t key{};
        value_t value{};
        entry_state state{EMPTY};
    };

    size_t _number_of_elements;
    size_t _table_size;
    std::vector<entry> _table;
    float _load_factor;
    float _max_load_factor;
    hash _hash;
    compare _compare;
    std::vector<std::pair<key_t, value_t>> _sorted_keys;
    bool _keys_dirty;
    unsigned int _comparisons;

    size_t get_next_prime(size_t x) {
        if (x % 2 == 0) {
            x++;
        }

        while (true) {
            bool is_prime = true;
            for (size_t i = 3; i <= std::sqrt(x); i += 2) {
                if (x % i == 0) {
                    is_prime = false;
                    break;
                }
            }

            if (is_prime) {
                return x;
            }

            x += 2;
        }
    }

    size_t hash_code(const key_t &k, const size_t i = 0) const {
        size_t hash1 = _hash(k);
        size_t hash2 = 1 + (hash1 % (_table_size - 1));
        return (hash1 + i * hash2) % _table_size;
    }

    void update_sorted_keys() {
        _sorted_keys.clear();
        _sorted_keys.reserve(_number_of_elements);

        for (const auto &entry : _table) {
            if (entry.state == OCCUPIED) {
                _sorted_keys.emplace_back(entry.key, entry.value);
            }
        }

        std::sort(_sorted_keys.begin(), _sorted_keys.end(),
                  [this](const std::pair<key_t, value_t> &a,
                         const std::pair<key_t, value_t> &b) -> bool {
                      return _compare(a.first, b.first);
                  });
        _keys_dirty = false;
    }

   public:
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

    ~open_hash_table() { clear(); }

    size_t bucket_count() const { return _table_size; }

    float load_factor() const {
        return static_cast<float>(_number_of_elements) / _table_size;
    }

    float max_load_factor() const { return _max_load_factor; }

    void load_factor(float lf) {
        if (lf <= 0 || lf > _max_load_factor) {
            throw std::out_of_range("load factor out of range");
        }

        _load_factor = lf;
        if (load_factor() > _load_factor) {
            rehash(2 * _table_size);
        }
    }

    void max_load_factor(float lf) {
        if (lf <= 0 || lf < _load_factor) {
            throw std::out_of_range("max load factor out of range");
        }
        _max_load_factor = lf;
    }

    void rehash(size_t new_size) {
        if (new_size <= _table_size) {
            return;
        }

        new_size = get_next_prime(new_size);
        std::vector<entry> new_table(new_size);

        for (size_t i = 0; i < _table_size; i++) {
            if (_table[i].state == OCCUPIED) {
                size_t j = 0;
                size_t index;
                do {
                    index =
                        (_hash(_table[i].key) +
                         j * (1 + (_hash(_table[i].key) % (new_size - 1)))) %
                        new_size;
                    j++;
                } while (new_table[index].state == OCCUPIED);

                new_table[index].key = _table[i].key;
                new_table[index].value = _table[i].value;
                new_table[index].state = OCCUPIED;
            }
        }

        _table_size = new_size;
        _table = std::move(new_table);
        _keys_dirty = true;
    }

    bool insert(const key_t &k, const value_t &v) {
        if (load_factor() > _max_load_factor) {
            rehash(2 * _table_size);
        }

        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);
            if (_table[index].state != OCCUPIED) {
                _table[index].key = k;
                _table[index].value = v;
                _table[index].state = OCCUPIED;
                _number_of_elements++;
                _keys_dirty = true;
                return true;
            } else if (_table[index].key == k) {
                return false;
            }
        } while (i < _table_size);

        return false;
    }

    bool remove(const key_t &k) {
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);
            if (_table[index].state == EMPTY) {
                return false;
            } else if (_table[index].state == OCCUPIED &&
                       _table[index].key == k) {
                _table[index].state = DELETED;
                _number_of_elements--;
                _keys_dirty = true;
                return true;
            }
        } while (i < _table_size);

        return false;
    }

    void clear() {
        _table.clear();
        _table.resize(_table_size);
        _number_of_elements = 0;
        _keys_dirty = true;
    }

    size_t size() const { return _number_of_elements; }

    bool empty() const { return _number_of_elements == 0; }

    value_t search(const key_t &k) const {
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);
            if (_table[index].state == EMPTY) {
                throw std::out_of_range("Key not found");
            } else if (_table[index].state == OCCUPIED &&
                       _table[index].key == k) {
                return _table[index].value;
            }
        } while (i < _table_size);

        throw std::out_of_range("Key not found");
    }

    bool contains(const key_t &k) const {
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);
            if (_table[index].state == EMPTY) {
                return false;
            } else if (_table[index].state == OCCUPIED &&
                       _table[index].key == k) {
                return true;
            }
        } while (i < _table_size);

        return false;
    }

    void att(const key_t &k, const value_t &v) {
        size_t i = 0;
        size_t index;
        do {
            index = hash_code(k, i++);
            if (_table[index].state == OCCUPIED && _table[index].key == k) {
                _table[index].value = v;
                return;
            }
        } while (i < _table_size);

        throw std::out_of_range("Key not found");
    }

    class iterator {
       private:
        const open_hash_table *_ht;
        size_t _index;

       public:
        iterator(const open_hash_table *ht, size_t index)
            : _ht(ht), _index(index) {}

        const std::pair<key_t, value_t> &operator*() const {
            return _ht->_sorted_keys[_index];
        }

        iterator &operator++() {
            _index++;
            return *this;
        }

        bool operator!=(const iterator &other) const {
            return _index != other._index;
        }

        std::pair<key_t, value_t> *operator->() const {
            return &_ht->_sorted_keys[_index];
        }
    };

    iterator begin() {
        if (_keys_dirty) {
            update_sorted_keys();
        }
        return iterator(this, 0);
    }

    iterator end() {
        if (_keys_dirty) {
            update_sorted_keys();
        }

        return iterator(this, _sorted_keys.size());
    }

    unsigned int comparisons() const { return _comparisons; }
};
