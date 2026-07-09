#ifndef CUSTOM_HASH_HPP
#define CUSTOM_HASH_HPP

#include <string>
/**
 * @brief Estructura genérica para implementar funciones hash personalizadas.
 *
 * Esta plantilla base debe ser especializada para los diferentes tipos de datos
 * que se deseen procesar.
 *
 * @tparam T Tipo de dato que se va a hashear.
 */
template <typename T>
struct CustomHash;
/**
 * @brief Especialización de CustomHash para enteros de 64 bits (long long).
 *
 * Diseñada específicamente para manejar identificadores numéricos (como user_id).
 * Utiliza el método de plegamiento (folding method) para calcular el hash.
 */
template <>
struct CustomHash<long long> {
    /**
     * @brief Calcula el hash de un número long long mediante plegamiento.
     * * El algoritmo toma el valor absoluto del número, lo divide en 4 bloques
     * de 16 bits cada uno, y aplica la operación bit a bit XOR (^) entre ellos.
     * * @param key El valor numérico (ID) a hashear.
     * @return unsigned long long El valor hash resultante.
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
 * @brief Especialización de CustomHash para cadenas de texto (std::string).
 *
 * Diseñada para manejar cadenas como nombres de usuario (user_screen_name).
 * Utiliza un método de suma de potencias basándose en los valores ASCII de 
 * los caracteres.
 */
template <>
struct CustomHash<std::string> {
    /**
     * @brief Calcula el hash de un std::string mediante suma de potencias ASCII.
     * * Itera sobre cada carácter de la cadena. Por cada carácter, calcula su valor 
     * ASCII elevado a una potencia correspondiente a su posición en la cadena (índice + 1).
     * Finalmente, suma todos estos valores.
     * * @param str La cadena de texto a hashear.
     * @return unsigned long long El valor hash acumulado.
     */
    unsigned long long operator()(const std::string& str) const {
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

#endif 