#ifndef HASH_ABIERTO_HPP
#define HASH_ABIERTO_HPP

/**
 * @file hash_abierto.hpp
 * @brief Definición e implementación de una Tabla Hash Abierta (Encadenamiento).
 * * Este archivo contiene la estructura necesaria para resolver colisiones en 
 * una tabla hash mediante el uso de listas doblemente enlazadas (chaining).
 */

#include <vector>
#include <list>
#include "custom_hash.hpp"

/**
 * @brief Estructura que representa un elemento almacenado en la Tabla Hash Abierta.
 * * Contiene la clave insertada y un contador que registra cuántas veces
 * se ha procesado dicha clave.
 * * @tparam K Tipo de dato de la clave (ej. long long, std::string).
 */
template <typename K>
struct EntryAbierto {
    K key;         ///< Clave única del elemento.
    int count;     ///< Número de ocurrencias de la clave.
    
    /**
     * @brief Constructor para inicializar una nueva entrada.
     * @param k El valor de la clave.
     * @param c El contador inicial de ocurrencias.
     */
    EntryAbierto(K k, int c) : key(k), count(c) {}
};

/**
 * @brief Clase que implementa una Tabla Hash con resolución de colisiones abierta.
 * * Utiliza un vector principal donde cada "bucket" (cubeta) es una 
 * lista doblemente enlazada (`std::list`). Si dos claves colisionan en 
 * el mismo índice, se encadenan en la lista de ese índice.
 * * @tparam K Tipo de dato de la clave.
 */
template <typename K>
class HashTableAbierto {
private:
    std::vector<std::list<EntryAbierto<K>>> table;  ///< Vector de listas para el encadenamiento.
    int size;                                       ///< Cantidad total de buckets en la tabla.

    /**
     * @brief Calcula el índice correspondiente para una clave específica.
     * * Instancia el functor `CustomHash` para obtener el valor hash bruto de 
     * la clave y luego aplica la operación módulo (`% size`) para ajustarlo 
     * a los límites del vector.
     * * @param key Clave a evaluar.
     * @return int Índice calculado (garantizado entre 0 y size - 1).
     */
    int hashFunction(const K& key) const {
        CustomHash<K> hasher;
        return hasher(key) % size; 
    }

public:
    /**
     * @brief Constructor que inicializa el tamaño de la tabla hash.
     * * @param tableSize Cantidad de buckets que tendrá la tabla. Se recomienda
     * un número primo o un tamaño suficientemente grande según el factor de carga esperado.
     */
    HashTableAbierto(int tableSize) : size(tableSize) {
        table.resize(size);
    }

    /**
     * @brief Procesa la inserción de una clave (ej. un tweet o usuario).
     * * Si la clave ya existe en la lista correspondiente a su índice, incrementa 
     * su contador de frecuencia en 1. Si no existe, crea una nueva entrada al 
     * final de la lista con un contador inicial de 1.
     * * @param key Clave que se desea insertar o actualizar.
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
     * @brief Consulta la cantidad de veces que una clave ha sido procesada.
     * * Calcula el índice de la clave, recorre la lista enlazada correspondiente 
     * y retorna el valor de su contador.
     * * @param key Clave a consultar.
     * @return int Frecuencia de la clave (0 si la clave no fue encontrada).
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
};

#endif 