#ifndef HASH_CERRADO_HPP
#define HASH_CERRADO_HPP

/**
 * @file hash_cerrado.hpp
 * @brief Definición e implementación de una Tabla Hash Cerrada (Direccionamiento Abierto).
 * * Este archivo contiene la estructura necesaria para resolver colisiones en 
 * una tabla hash mediante diferentes estrategias de sondeo (lineal, cuadrático y doble hashing).
 */

#include <vector>
#include <iostream>
#include "custom_hash.hpp"

/**
 * @brief Estados posibles para una celda dentro de la tabla hash cerrada.
 */
enum State { 
    EMPTY,     ///< La celda nunca ha sido utilizada.
    OCCUPIED,  ///< La celda contiene una clave válida actualmente.
    DELETED    ///< La celda tenía un elemento que fue eliminado (Tombstone).
};

/**
 * @brief Estrategias de sondeo disponibles para la resolución de colisiones.
 */
enum ProbingStrategy { 
    LINEAR,     ///< Sondeo Lineal (saltos secuenciales).
    QUADRATIC,  ///< Sondeo Cuadrático (saltos polinómicos).
    DOUBLE      ///< Doble Hashing (saltos basados en una segunda función hash).
};

/**
 * @brief Estructura que representa una celda (bucket) en la Tabla Hash Cerrada.
 * @tparam K Tipo de dato de la clave.
 */
template <typename K>
struct EntryCerrado {
    K key;          ///< Clave almacenada en la celda.
    int count;      ///< Frecuencia de aparición de la clave.
    State state;    ///< Estado actual de la celda.
    
    /**
     * @brief Constructor por defecto.
     * Inicializa el contador en 0 y el estado como EMPTY.
     */
    EntryCerrado() : count(0), state(EMPTY) {}
};

/**
 * @brief Clase que implementa una Tabla Hash con resolución de colisiones por direccionamiento abierto.
 * @tparam K Tipo de dato de la clave.
 */
template <typename K>
class HashTableCerrado {
private:
    std::vector<EntryCerrado<K>> table; ///< Vector principal que almacena las celdas.
    int size;                           ///< Tamaño total de la tabla (capacidad).
    ProbingStrategy strategy;           ///< Estrategia de sondeo configurada.
    CustomHash<K> hasher;               ///< Instancia del functor para calcular hashes.
    
    /**
     * @brief Función hash primaria.
     * @param key Clave a evaluar.
     * @return int Índice base calculado.
     */
    int h1(const K& key) {
        return hasher(key) % size;
    }

    /**
     * @brief Función hash secundaria, utilizada exclusivamente para Doble Hashing.
     * Asegura que el tamaño del salto nunca sea 0.
     * @param key Clave a evaluar.
     * @return int Tamaño del salto (step).
     */
    int h2(const K& key) {
        unsigned long long raw_hash = hasher(key);
        int step = (raw_hash / size) % size; 
        return (step == 0) ? 1 : step;
    }

    /**
     * @brief Calcula el índice utilizando Sondeo Lineal.
     * @param key Clave original.
     * @param i Número de intento (colisión actual).
     * @return int Nuevo índice propuesto.
     */
    int linear_probing(const K& key, int i) {
        return (h1(key) + i) % size;
    }

    /**
     * @brief Calcula el índice utilizando Sondeo Cuadrático.
     * @param key Clave original.
     * @param i Número de intento (colisión actual).
     * @return int Nuevo índice propuesto.
     */
    int quadratic_probing(const K& key, int i) {
        return (h1(key) + i + 2 * i * i) % size;
    }

    /**
     * @brief Calcula el índice utilizando Doble Hashing.
     * @param key Clave original.
     * @param i Número de intento (colisión actual).
     * @return int Nuevo índice propuesto.
     */
    int double_hashing(const K& key, int i) {
        return (h1(key) + i * h2(key)) % size;
    }

public:

    /**
     * @brief Constructor de la Tabla Hash Cerrada.
     * @param tableSize Capacidad máxima de la tabla.
     * @param prob_strategy Estrategia a utilizar para resolver colisiones.
     */
    HashTableCerrado(int tableSize, ProbingStrategy prob_strategy) 
        : size(tableSize), strategy(prob_strategy) {
        table.resize(size);
    }

    /**
     * @brief Función de enrutamiento que delega el cálculo del índice según la estrategia elegida.
     * @param key Clave a insertar o buscar.
     * @param i Número del intento actual.
     * @return int Índice calculado.
     */
    int probe(const K& key, int i) {
        switch(strategy) {
            case LINEAR: return linear_probing(key, i);
            case QUADRATIC: return quadratic_probing(key, i);
            case DOUBLE: return double_hashing(key, i);
        }
        return 0;
    }

    /**
     * @brief Inserta una clave nueva o incrementa su contador si ya existe.
     * Utiliza la estrategia de sondeo para encontrar la clave (y actualizarla) o 
     * una celda disponible (EMPTY o DELETED) para hacer la inserción.
     * @param key Clave a procesar.
     */
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

    /**
     * @brief Busca la frecuencia (contador) de una clave en la tabla.
     * @param key Clave a buscar.
     * @return int Número de veces que se ha procesado la clave (0 si no existe).
     */
    int getTweetCount(const K& key) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);

            // Un espacio vacío indica que la clave no puede estar más adelante
            if (table[index].state == EMPTY) break; 
            
            if (table[index].state == OCCUPIED && table[index].key == key) {
                return table[index].count;
            }
        }
        return 0;
    }
};

#endif 