/**
 * @file hash_abierto.cpp
 * @brief Implementación de una Tabla Hash con resolución de colisiones mediante 
 * encadenamiento (Hash Abierto).
 */

#include <iostream>
#include <vector>
#include <list>
#include <string>

using namespace std;
// ============================================================================
// Funciones hash personalizadas para tipos de clave específicos
// ============================================================================

/**
 * @brief Estructura genérica para implementar funciones hash personalizadas.
 * @tparam K Tipo de dato de la clave.
 */
template <typename K>
struct CustomHash;
/**
 * @brief Especialización de CustomHash para cadenas de texto (std::string).
 * * Calcula el hash acumulando el valor de cada carácter elevado a la potencia
 * de su posición (índice + 1).
 */
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

/**
 * @brief Especialización de CustomHash para enteros de 64 bits (long long).
 * * Utiliza el método de plegamiento (folding) dividiendo el valor absoluto
 * de la clave en bloques de 16 bits y aplicando la operación XOR.
 */
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
// ============================================================================
// Estructura de la Tabla Hash con encadenamiento (Hash Abierto)
// ============================================================================

/**
 * @brief Representa un elemento almacenado en la Tabla Hash.
 * * Contiene la clave y un contador de frecuencia (útil para contar
 * repeticiones de un mismo ID o usuario en el dataset).
 * * @tparam K Tipo de dato de la clave.
 */
template <typename K>
struct Entry {
    K key;      ///< Clave única del elemento.
    int count;  ///< Contador de ocurrencias de la clave.
    /**
     * @brief Constructor de la entrada.
     * @param k Clave a almacenar.
     * @param c Contador inicial.
     */
    Entry(K k, int c) : key(k), count(c) {}
};

/**
 * @brief Implementación de una Tabla Hash Abierta (Encadenamiento).
 * * Utiliza un vector de listas doblemente enlazadas (std::list) para manejar 
 * las colisiones. Si dos claves generan el mismo índice, ambas se añaden 
 * a la lista correspondiente de ese índice.
 * * @tparam K Tipo de dato de la clave.
 */
template <typename K>
class HashTable {
private:
    vector<list<Entry<K>>> table;   ///< Contenedor principal: Array de listas.
    int size;                       ///< Tamaño del array (cantidad de "buckets").

    /**
     * @brief Calcula el índice en la tabla para una clave dada.
     * * Aplica la función hash especializada según el tipo K y ajusta el 
     * resultado al tamaño actual de la tabla usando el operador módulo.
     * * @param key Clave a evaluar.
     * @return int Índice calculado dentro de los límites de la tabla.
     */
    int hashFunction(const K& key) const {
        CustomHash<K> hasher;
        return hasher(key) % size; 
    }

public:
    /**
     * @brief Constructor de la Tabla Hash.
     * @param tableSize Tamaño inicial del array (cantidad de buckets).
     */
    HashTable(int tableSize) : size(tableSize) {
        table.resize(size);
    }

    /**
     * @brief Inserta una nueva clave o incrementa su contador si ya existe.
     * * Calcula el índice de la clave. Si la clave ya se encuentra en la lista 
     * enlazada de ese índice, incrementa su contador. Si no existe, la añade 
     * al final de la lista con un contador inicial de 1.
     * * @param key Clave a insertar o actualizar.
     */
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

    /**
     * @brief Obtiene la cantidad de ocurrencias registradas para una clave.
     * * Busca la clave en la lista enlazada correspondiente a su índice hash.
     * * @param key Clave a buscar.
     * @return int Número de ocurrencias de la clave (0 si no existe).
     */
    int getTweetCount(const K& key) const {
        int index = hashFunction(key);

        for (const auto& entry : table[index]) {
            if (entry.key == key) {
                return entry.count;
            }
        }
        return 0;
    }

    /**
     * @brief Elimina una clave y su registro completo de la tabla.
     * * Busca la clave en su respectiva lista enlazada. Si la encuentra, 
     * elimina el nodo completo de la lista.
     * * @param key Clave a eliminar.
     */
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

