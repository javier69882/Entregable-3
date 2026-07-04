#ifndef CUSTOM_HASH_HPP
#define CUSTOM_HASH_HPP

#include <string>

// Plantilla genérica
template <typename T>
struct CustomHash;

//long long (user_id) - Método Plegamiento
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

//strings (user_screen_name) - Suma de potencias ASCII
template <>
struct CustomHash<std::string> {
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