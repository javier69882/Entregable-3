#ifndef HASH_CERRADO_HPP
#define HASH_CERRADO_HPP

#include <vector>
#include <iostream>
#include "custom_hash.hpp"

enum State { EMPTY, OCCUPIED, DELETED };
enum ProbingStrategy { LINEAR, QUADRATIC, DOUBLE };

template <typename K>
struct EntryCerrado {
    K key;
    int count;
    State state;

    EntryCerrado() : count(0), state(EMPTY) {}
};

template <typename K>
class HashTableCerrado {
private:
    std::vector<EntryCerrado<K>> table;
    int size;
    ProbingStrategy strategy;
    CustomHash<K> hasher;

    int h1(const K& key) {
        return hasher(key) % size;
    }

    int h2(const K& key) {
        unsigned long long raw_hash = hasher(key);
        int step = (raw_hash / size) % size; 
        return (step == 0) ? 1 : step;
    }

    int linear_probing(const K& key, int i) {
        return (h1(key) + i) % size;
    }

    int quadratic_probing(const K& key, int i) {
        return (h1(key) + i + 2 * i * i) % size;
    }

    int double_hashing(const K& key, int i) {
        return (h1(key) + i * h2(key)) % size;
    }

public:
    HashTableCerrado(int tableSize, ProbingStrategy prob_strategy) 
        : size(tableSize), strategy(prob_strategy) {
        table.resize(size);
    }

    int probe(const K& key, int i) {
        switch(strategy) {
            case LINEAR: return linear_probing(key, i);
            case QUADRATIC: return quadratic_probing(key, i);
            case DOUBLE: return double_hashing(key, i);
        }
        return 0;
    }

    void processTweet(const K& key) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);

            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].count++;
                return;
            }

            if (table[index].state == EMPTY || table[index].state == DELETED) {
                table[index].key = key;
                table[index].count = 1;
                table[index].state = OCCUPIED;
                return;
            }
        }
        std::cout << "Tabla llena, no se pudo procesar." << std::endl;
    }

    int getTweetCount(const K& key) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);
            if (table[index].state == EMPTY) break; 
            if (table[index].state == OCCUPIED && table[index].key == key) {
                return table[index].count;
            }
        }
        return 0;
    }
};

#endif 