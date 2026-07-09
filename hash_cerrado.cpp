/**
 * @file hash_cerrado.cpp
 * @brief Implementación de una Tabla Hash con resolución de colisiones mediante 
 * direccionamiento abierto (Hash Cerrado). Incluye las estructuras base en un 
 * único archivo.
 */

#include <iostream>
#include <vector>
#include <string>

using namespace std;

/**
 * @brief Representa el estado lógico de una celda dentro de la tabla hash.
 * * Utiliza el concepto de "Tombstone" (DELETED) para asegurar que las 
 * secuencias de sondeo no se rompan prematuramente al eliminar elementos.
 */
enum State { 
    EMPTY,      ///< La celda nunca ha sido utilizada y está vacía.
    OCCUPIED,   ///< La celda contiene una clave válida actualmente.
    DELETED     ///< La celda fue utilizada, pero su elemento fue "borrado". 
};

/**
 * @brief Define las diferentes estrategias para resolver colisiones.
 */
enum ProbingStrategy { 
    LINEAR,     ///< Sondeo Lineal (búsqueda secuencial).
    QUADRATIC,  ///< Sondeo Cuadrático (saltos exponenciales).
    DOUBLE      ///< Doble Hashing (saltos basados en un segundo hash).
};

// ============================================================================
// Funciones hash personalizadas para tipos de clave específicos
// ============================================================================

/**
 * @brief Estructura genérica para implementar funciones hash personalizadas.
 * @tparam T Tipo de dato de la clave.
 */
template <typename T>
struct CustomHash;

/**
 * @brief Especialización de CustomHash para enteros de 64 bits (user_id).
 * * Utiliza el método de plegamiento (Folding Method).
 */
template <>
struct CustomHash<long long> {
    /**
     * @brief Calcula el hash de un ID de usuario numérico.
     * Toma el valor absoluto, lo divide en bloques de 16 bits y aplica XOR.
     * @param key ID numérico del usuario.
     * @return unsigned long long Valor hash calculado.
     */
    unsigned long long operator()(long long key) const {
        unsigned long long x = static_cast<unsigned long long>(key < 0 ? -key : key);
        unsigned long long bloque1 = x & 0xFFFF;          
        unsigned long long bloque2 = (x >> 16) & 0xFFFF;   
        unsigned long long bloque3 = (x >> 32) & 0xFFFF;   
        unsigned long long bloque4 = (x >> 48) & 0xFFFF;   
        return bloque1 ^ bloque2 ^ bloque3 ^ bloque4;
    }
};

/**
 * @brief Especialización de CustomHash para cadenas de texto (user_screen_name).
 * * Basado en la suma ponderada de valores ASCII.
 */
template <>
struct CustomHash<string> {
    /**
     * @brief Calcula el hash iterando sobre cada carácter, elevando su valor
     * ASCII a la potencia de su posición relativa en el string.
     * @param str Nombre de usuario a procesar.
     * @return unsigned long long Valor hash resultante.
     */
    unsigned long long operator()(const string& str) const {
        unsigned long long hash = 0;
        for (size_t i = 0; i < str.length(); ++i) {
            unsigned long long charVal = static_cast<unsigned long long>(str[i]);
            unsigned long long powerVal = 1;


            for (size_t j = 0; j < i + 1; ++j) {
                powerVal *= charVal;
            }
            hash += powerVal;
        }
        return hash;
    }
};
// ============================================================================
// Estructura de la Tabla Hash con direccionamiento abierto (Hash Cerrado)
// ============================================================================

/**
 * @brief Representa un "bucket" o celda de la tabla hash.
 * @tparam K Tipo de dato de la clave.
 */
template <typename K>
struct Entry {
    K key;          ///< La clave almacenada.
    int count;      ///< Frecuencia con la que se ha registrado la clave.
    State state;    ///< Estado actual (EMPTY, OCCUPIED, DELETED).

    /**
     * @brief Constructor por defecto. Inicializa como vacío y contador en 0.
     */
    Entry() : count(0), state(EMPTY) {}
};
/**
 * @brief Implementación de la Tabla Hash Cerrada.
 * * Maneja las colisiones calculando nuevos índices dentro del mismo arreglo
 * en base a la estrategia de sondeo seleccionada.
 * @tparam K Tipo de dato de la clave.
 */
template <typename K>
class HashTable {
private:
    vector<Entry<K>> table;     ///< Contenedor dinámico principal.
    int size;                   ///< Tamaño de la tabla (cantidad de cubetas).
    ProbingStrategy strategy;   ///< Método seleccionado para resolver colisiones.
    CustomHash<K> hasher;       ///< Objeto functor para invocar el hash.

    /**
     * @brief Función hash primaria.
     * @param key Clave de entrada.
     * @return int Índice inicial.
     */
    int h1(const K& key) {
        return hasher(key) % size;
    }

    /**
     * @brief Función hash secundaria, exclusiva para el método Double Hashing.
     * @param key Clave de entrada.
     * @return int Tamaño del salto (garantizado para ser mayor que 0).
     */
    int h2(const K& key) {
        unsigned long long raw_hash = hasher(key);
        int step = (raw_hash / size) % size; 
        return (step == 0) ? 1 : step;
    }

    /**
     * @brief Calcula el próximo índice usando Sondeo Lineal.
     * @param key Clave original.
     * @param i Intento de colisión actual.
     * @return int Siguiente índice a revisar.
     */
    int linear_probing(const K& key, int i) {
        return (h1(key) + i) % size;
    }

    /**
     * @brief Calcula el próximo índice usando Sondeo Cuadrático.
     * @param key Clave original.
     * @param i Intento de colisión actual.
     * @return int Siguiente índice a revisar.
     */
    int quadratic_probing(const K& key, int i) {
        return (h1(key) + i + 2 * i * i) % size;
    }

    /**
     * @brief Calcula el próximo índice usando Doble Hashing.
     * @param key Clave original.
     * @param i Intento de colisión actual.
     * @return int Siguiente índice a revisar.
     */
    int double_hashing(const K& key, int i) {
        return (h1(key) + i * h2(key)) % size;
    }


public:
    /**
     * @brief Construye una nueva Tabla Hash Cerrada.
     * @param tableSize Capacidad total de la tabla.
     * @param prob_strategy Estrategia elegida para manejar colisiones.
     */
    HashTable(int tableSize, ProbingStrategy prob_strategy) 
        : size(tableSize), strategy(prob_strategy) {
        table.resize(size);
    }

    /**
     * @brief Interfaz para invocar el cálculo de sondeo según la configuración.
     * @param key Clave a insertar o buscar.
     * @param i Número de intento actual.
     * @return int Índice propuesto en la tabla.
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
     * @brief Registra un tweet en la tabla actualizando los contadores.
     * * Busca la celda que le corresponde a la clave. Si la clave ya existe, 
     * suma 1 a su contador. Si encuentra un espacio libre (EMPTY o DELETED), 
     * lo ocupa con la nueva clave.
     * * @param key Clave (usuario o ID) a registrar.
     */
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

    /**
     * @brief Recupera la cantidad total de ocurrencias de una clave.
     * @param key Clave que se desea buscar.
     * @return int Número de tweets/apariciones (retorna 0 si no se encuentra).
     */
    int getTweetCount(const K& key) {
        for (int i = 0; i < size; i++) {
            int index = probe(key, i);

            // Un espacio vacío significa que el elemento no puede estar más allá
            if (table[index].state == EMPTY) break; 

            if (table[index].state == OCCUPIED && table[index].key == key) {
                return table[index].count;
            }
        }
        return 0;
    }

    /**
     * @brief Imprime el contenido actual de la tabla en consola.
     * * Muestra únicamente las celdas que están en estado OCCUPIED. 
     * Ideal para depurar el estado del sistema.
     */
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