#ifndef HASH_ABIERTO_HPP
#define HASH_ABIERTO_HPP

#include <vector>
#include <list>
#include "custom_hash.hpp"

template <typename K>
struct EntryAbierto {
    K key;
    int count;

    EntryAbierto(K k, int c) : key(k), count(c) {}
};

template <typename K>
class HashTableAbierto {
private:
    std::vector<std::list<EntryAbierto<K>>> table;
    int size;

    int hashFunction(const K& key) const {
        CustomHash<K> hasher;
        return hasher(key) % size; 
    }

public:
    HashTableAbierto(int tableSize) : size(tableSize) {
        table.resize(size);
    }

    
    void processTweet(const K& key) {
        int index = hashFunction(key);

        for (auto& entry : table[index]) {
            if (entry.key == key) {
                entry.count++;
                return;
            }
        }
        table[index].emplace_back(key, 1);
    }

    
    int getTweetCount(const K& key) const {
        int index = hashFunction(key);

        for (const auto& entry : table[index]) {
            if (entry.key == key) {
                return entry.count;
            }
        }
        return 0;
    }
};

#endif 