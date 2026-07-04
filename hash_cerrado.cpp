#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum State { EMPTY, OCCUPIED, DELETED };
enum ProbingStrategy { LINEAR, QUADRATIC, DOUBLE };


template <typename T>
struct CustomHash;

// especialización para int (user_id)
template <>
struct CustomHash<long long> {
    unsigned long long operator()(long long key) const {
        unsigned long long x = static_cast<unsigned long long>(key < 0 ? -key : key);
        unsigned long long bloque1 = x & 0xFFFF;          
        unsigned long long bloque2 = (x >> 16) & 0xFFFF;   
        unsigned long long bloque3 = (x >> 32) & 0xFFFF;   
        unsigned long long bloque4 = (x >> 48) & 0xFFFF;   
        return bloque1 ^ bloque2 ^ bloque3 ^ bloque4;
    }
};

// especialización para strings (user_screen_name)
template <>
struct CustomHash<string> {
    unsigned long long operator()(const string& str) const {
        unsigned long long hash = 0;
        for (size_t i = 0; i < str.length(); ++i) {
            unsigned long long charVal = static_cast<unsigned long long>(str[i]);
            unsigned long long powerVal = 1;

            // Calculamos charVal elevado a (i + 1)
            // (Nota: Esto desbordará rápido, pero el wrap-around es válido en hashing)
            for (size_t j = 0; j < i + 1; ++j) {
                powerVal *= charVal;
            }
            hash += powerVal;
        }
        return hash;
    }
};

// entrada de la tabla, genérica para cualquier clave k
template <typename K>
struct Entry {
    K key;
    int count;
    State state;

    Entry() : count(0), state(EMPTY) {}
};

// clase tabla hash genérica
template <typename K>
class HashTable {
private:
    vector<Entry<K>> table;
    int size;
    ProbingStrategy strategy;
    CustomHash<K> hasher; // instancia del struct de hashing

    // función hash principal
    int h1(const K& key) {
        return hasher(key) % size;
    }

    // función hash secundaria para double hashing
    int h2(const K& key) {
        unsigned long long raw_hash = hasher(key);
        int step = (raw_hash / size) % size; 
        return (step == 0) ? 1 : step;
    }

    // sondeos
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
    HashTable(int tableSize, ProbingStrategy prob_strategy) 
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

    // lógica para contar los tweets
    void processTweet(const K& key) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);

            // Si ya existe
            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].count++;
                return;
            }

            // Si encontramos un lugar vacío o borrado
            if (table[index].state == EMPTY || table[index].state == DELETED) {
                table[index].key = key;
                table[index].count = 1;
                table[index].state = OCCUPIED;
                return;
            }
        }
        cout << "Tabla llena, no se pudo procesar." << endl;
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

    void print() {
        for (int i = 0; i < size; i++) {
            if (table[i].state == OCCUPIED) {
                cout << "Index " << i << " | Key: " << table[i].key 
                     << " -> Tweets: " << table[i].count << endl;
            }
        }
        cout << "------------------------\n";
    }
};