#include <iostream>
#include <vector>
#include <list>
#include <string>

using namespace std;

// Funciones hash para strings y enteros (IDs de usuario)

template <typename K>
struct CustomHash;

// Especialización para strings: ASCII^1 + ASCII^2 + ASCII^3 ...
template <>
struct CustomHash<string> {
    unsigned long long operator()(const string& str) const {
        unsigned long long hash = 0;
        
        for (size_t i = 0; i < str.length(); ++i) {
            unsigned long long charVal = static_cast<unsigned long long>(str[i]);
            unsigned long long powerVal = 1;
            
            // Calculamos charVal elevado a (i + 1)
            for (size_t j = 0; j < i + 1; ++j) {
                powerVal *= charVal;
            }
            
            hash += powerVal;
        }
        
        return hash;
    }
};

// enteros (IDs de usuario): Método de Plegamiento (Folding)
template <>
struct CustomHash<long long> {
    unsigned long long operator()(long long key) const {
        // Nos aseguramos de trabajar con un valor positivo
        unsigned long long x = static_cast<unsigned long long>(key < 0 ? -key : key);
        
        // Dividimos el número de 64 bits en 4 bloques de 16 bits cada uno
        unsigned long long bloque1 = x & 0xFFFF;          // Bits del 0 al 15
        unsigned long long bloque2 = (x >> 16) & 0xFFFF;   // Bits del 16 al 31
        unsigned long long bloque3 = (x >> 32) & 0xFFFF;   // Bits del 32 to 47
        unsigned long long bloque4 = (x >> 48) & 0xFFFF;   // Bits del 48 al 63
        
        // "Plegamos" los bloques combinándolos con la operación XOR
        return bloque1 ^ bloque2 ^ bloque3 ^ bloque4;
    }
};

// --- 2. ESTRUCTURAS DE LA TABLA HASH ---

template <typename K>
struct Entry {
    K key;
    int count;

    Entry(K k, int c) : key(k), count(c) {}
};

template <typename K>
class HashTable {
private:
    vector<list<Entry<K>>> table;
    int size;

    // Aplica nuestra función hash personalizada y luego el módulo del vector
    int hashFunction(const K& key) const {
        CustomHash<K> hasher;
        return hasher(key) % size; 
    }

public:
    HashTable(int tableSize) : size(tableSize) {
        table.resize(size);
    }

    void registrarTweet(const K& key) {
        int index = hashFunction(key);

        for (auto& entry : table[index]) {
            if (entry.key == key) {
                entry.count++;
                return;
            }
        }
        table[index].emplace_back(key, 1);
    }

    int obtenerConteo(const K& key) const {
        int index = hashFunction(key);

        for (const auto& entry : table[index]) {
            if (entry.key == key) {
                return entry.count;
            }
        }
        return 0;
    }

    void remove(const K& key) {
        int index = hashFunction(key);

        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->key == key) {
                table[index].erase(it);
                return;
            }
        }
    }
};

