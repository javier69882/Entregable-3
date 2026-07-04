#ifndef _WIN32
#include <malloc.h>
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>

//estructuras
#include "parserdataset.hpp"
#include "hash_abierto.hpp"
#include "hash_cerrado.hpp"

using namespace std;


//FUNCIÓN MULTIPLATAFORMA PARA MEDIR MEMORIA (WSL / WINDOWS)

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>

static size_t getCurrentProcessMemoryBytes() {
    PROCESS_MEMORY_COUNTERS_EX counters{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters), sizeof(counters))) {
        return static_cast<size_t>(counters.WorkingSetSize);
    }
    return 0;
}
#else
#include <unistd.h>

static size_t getCurrentProcessMemoryBytes() {
    size_t memorySize = 0;
    ifstream file("/proc/self/statm");
    if (file) {
        size_t size, resident, share, text, lib, data, dt;
        file >> size >> resident >> share >> text >> lib >> data >> dt;
        memorySize = resident * sysconf(_SC_PAGE_SIZE); 
    }
    return memorySize;
}
#endif


//UTILIDADES PARA EXTRAER LA CLAVE CORRECTA SEGÚN EL TEMPLATE, PERMITIENDO USAR EL MISMO CÓDIGO PARA AMBOS TIPOS DE CLAVE

template <typename K> K extraerClave(const TweetData& tweet);

template <> long long extraerClave<long long>(const TweetData& tweet) {
    return tweet.user_id;
}

template <> string extraerClave<string>(const TweetData& tweet) {
    return tweet.user_screen_name;
}

// EXPERIMENTO DE RENDIMIENTO (TIEMPOS DE EJECUCIÓN CADA 10K)

template <typename K>
void experimentoTiempos(const vector<TweetData>& dataset, const string& archivo_salida, const string& nombre_dataset) {
    ofstream archivo(archivo_salida);
    if (!archivo.is_open()) return;

    //MEDIMOS EN MICROSEGUNDOS 
    archivo << "numero_experimento;dataset;estructura_de_datos;cantidad_consultas;tiempo_ejecucion_us\n";

    int repeticiones = 20; 
    int table_size = 300000; 

    cout << "Iniciando experimento de tiempos (" << repeticiones << " repeticiones) -> " << archivo_salida << endl;

    for (int exp = 1; exp <= repeticiones; exp++) {
        cout << "  Ejecutando repeticion " << exp << "..." << endl;

        //Hash Abierto 
        {
            HashTableAbierto<K> hashAbierto(table_size);
            long long tiempo_acumulado = 0;
            auto inicio = chrono::high_resolution_clock::now();

            for (size_t i = 0; i < dataset.size(); ++i) {
                hashAbierto.processTweet(extraerClave<K>(dataset[i]));

                if ((i + 1) % 10000 == 0) {
                    auto fin = chrono::high_resolution_clock::now();
                    tiempo_acumulado += chrono::duration_cast<chrono::microseconds>(fin - inicio).count();
                    archivo << exp << ";" << nombre_dataset << ";Hash_Abierto;" << (i + 1) << ";" << tiempo_acumulado << "\n";
                    inicio = chrono::high_resolution_clock::now();
                }
            }
        } 

        // Hash Cerrado - Linear Probing 
        {
            HashTableCerrado<K> hashLineal(table_size, LINEAR);
            long long tiempo_acumulado = 0;
            auto inicio = chrono::high_resolution_clock::now();

            for (size_t i = 0; i < dataset.size(); ++i) {
                hashLineal.processTweet(extraerClave<K>(dataset[i]));

                if ((i + 1) % 10000 == 0) {
                    auto fin = chrono::high_resolution_clock::now();
                    tiempo_acumulado += chrono::duration_cast<chrono::microseconds>(fin - inicio).count();
                    archivo << exp << ";" << nombre_dataset << ";Cerrado_Lineal;" << (i + 1) << ";" << tiempo_acumulado << "\n";
                    inicio = chrono::high_resolution_clock::now();
                }
            }
        }

        //Hash Cerrado - Quadratic Probing 
        {
            HashTableCerrado<K> hashCuadratico(table_size, QUADRATIC);
            long long tiempo_acumulado = 0;
            auto inicio = chrono::high_resolution_clock::now();

            for (size_t i = 0; i < dataset.size(); ++i) {
                hashCuadratico.processTweet(extraerClave<K>(dataset[i]));

                if ((i + 1) % 10000 == 0) {
                    auto fin = chrono::high_resolution_clock::now();
                    tiempo_acumulado += chrono::duration_cast<chrono::microseconds>(fin - inicio).count();
                    archivo << exp << ";" << nombre_dataset << ";Cerrado_Cuadratico;" << (i + 1) << ";" << tiempo_acumulado << "\n";
                    inicio = chrono::high_resolution_clock::now();
                }
            }
        }

        //Hash Cerrado - Double Hashing 
        {
            HashTableCerrado<K> hashDoble(table_size, DOUBLE);
            long long tiempo_acumulado = 0;
            auto inicio = chrono::high_resolution_clock::now();

            for (size_t i = 0; i < dataset.size(); ++i) {
                hashDoble.processTweet(extraerClave<K>(dataset[i]));

                if ((i + 1) % 10000 == 0) {
                    auto fin = chrono::high_resolution_clock::now();
                    tiempo_acumulado += chrono::duration_cast<chrono::microseconds>(fin - inicio).count();
                    archivo << exp << ";" << nombre_dataset << ";Cerrado_Doble;" << (i + 1) << ";" << tiempo_acumulado << "\n";
                    inicio = chrono::high_resolution_clock::now();
                }
            }
        }

        //Implementación STL (std::unordered_map) 
        {
            unordered_map<K, int> stlMap;
            stlMap.reserve(table_size); 
            long long tiempo_acumulado = 0;
            auto inicio = chrono::high_resolution_clock::now();

            for (size_t i = 0; i < dataset.size(); ++i) {
                stlMap[extraerClave<K>(dataset[i])]++;

                if ((i + 1) % 10000 == 0) {
                    auto fin = chrono::high_resolution_clock::now();
                    tiempo_acumulado += chrono::duration_cast<chrono::microseconds>(fin - inicio).count();
                    archivo << exp << ";" << nombre_dataset << ";STL_UnorderedMap;" << (i + 1) << ";" << tiempo_acumulado << "\n";
                    inicio = chrono::high_resolution_clock::now();
                }
            }
        }
    }
    archivo.close();
    cout << "Experimento de tiempos finalizado.\n" << endl;
}

//EXPERIMENTO DE MEMORIA RAM

template <typename K>
void medirMemoriaEstructura(const vector<TweetData>& dataset, ofstream& archivo, const string& nombre_estructura, const string& tipo_clave) {
    int table_size = 300000;
    
    //Obligar a Linux a devolver la memoria residual al SO antes de medir
#ifndef _WIN32
    malloc_trim(0); 
#endif

    //Medir RAM base ANTES de crear la estructura
    size_t mem_antes = getCurrentProcessMemoryBytes();

    //Crear y llenar la estructura
    {
        if (nombre_estructura == "Hash_Abierto") {
            HashTableAbierto<K> tabla(table_size);
            for (const auto& d : dataset) tabla.processTweet(extraerClave<K>(d));
            size_t mem_despues = getCurrentProcessMemoryBytes();
            double mb_usados = (mem_despues > mem_antes) ? (mem_despues - mem_antes) / (1024.0 * 1024.0) : 0;
            archivo << nombre_estructura << ";" << tipo_clave << ";" << mb_usados << "\n";
            
        } else if (nombre_estructura == "Cerrado_Lineal") {
            HashTableCerrado<K> tabla(table_size, LINEAR);
            for (const auto& d : dataset) tabla.processTweet(extraerClave<K>(d));
            size_t mem_despues = getCurrentProcessMemoryBytes();
            double mb_usados = (mem_despues > mem_antes) ? (mem_despues - mem_antes) / (1024.0 * 1024.0) : 0;
            archivo << nombre_estructura << ";" << tipo_clave << ";" << mb_usados << "\n";

        } else if (nombre_estructura == "Cerrado_Cuadratico") {
            HashTableCerrado<K> tabla(table_size, QUADRATIC);
            for (const auto& d : dataset) tabla.processTweet(extraerClave<K>(d));
            size_t mem_despues = getCurrentProcessMemoryBytes();
            double mb_usados = (mem_despues > mem_antes) ? (mem_despues - mem_antes) / (1024.0 * 1024.0) : 0;
            archivo << nombre_estructura << ";" << tipo_clave << ";" << mb_usados << "\n";

        } else if (nombre_estructura == "Cerrado_Doble") {
            HashTableCerrado<K> tabla(table_size, DOUBLE);
            for (const auto& d : dataset) tabla.processTweet(extraerClave<K>(d));
            size_t mem_despues = getCurrentProcessMemoryBytes();
            double mb_usados = (mem_despues > mem_antes) ? (mem_despues - mem_antes) / (1024.0 * 1024.0) : 0;
            archivo << nombre_estructura << ";" << tipo_clave << ";" << mb_usados << "\n";

        } else if (nombre_estructura == "STL_UnorderedMap") {
            unordered_map<K, int> tabla;
            tabla.reserve(table_size);
            for (const auto& d : dataset) tabla[extraerClave<K>(d)]++;
            size_t mem_despues = getCurrentProcessMemoryBytes();
            double mb_usados = (mem_despues > mem_antes) ? (mem_despues - mem_antes) / (1024.0 * 1024.0) : 0;
            archivo << nombre_estructura << ";" << tipo_clave << ";" << mb_usados << "\n";
        }
    } 
}
template <typename K>
void experimentoMemoria(const vector<TweetData>& dataset, const string& archivo_salida, const string& tipo_clave) {
    // Abrimos en modo append (ios::app) por si usamos el mismo archivo para varias llamadas
    ofstream archivo(archivo_salida, ios::app);
    if (!archivo.is_open()) {
        cerr << "Error al crear el archivo CSV de memoria." << endl;
        return;
    }

    cout << "Midiendo memoria para tipo de clave: " << tipo_clave << "..." << endl;

    medirMemoriaEstructura<K>(dataset, archivo, "Hash_Abierto", tipo_clave);
    medirMemoriaEstructura<K>(dataset, archivo, "Cerrado_Lineal", tipo_clave);
    medirMemoriaEstructura<K>(dataset, archivo, "Cerrado_Cuadratico", tipo_clave);
    medirMemoriaEstructura<K>(dataset, archivo, "Cerrado_Doble", tipo_clave);
    medirMemoriaEstructura<K>(dataset, archivo, "STL_UnorderedMap", tipo_clave);

    archivo.close();
    cout << "Medicion de memoria finalizada.\n" << endl;
}




int main() {
    cout << "========================================\n";
    cout << "   INICIANDO EXPERIMENTOS\n";
    cout << "========================================\n\n";

    //Cargar el dataset
    cout << "[1/3] Cargando dataset 'auspol2019.csv'..." << endl;
    
    // Ajustado según la estructura de carpetas de tu proyecto
    vector<TweetData> tweets = cargarDataset("dataset/auspol2019.csv"); 

    if (tweets.empty()) {
        cerr << "Error: El dataset no se pudo cargar o esta vacio. Verifica la ruta." << endl;
        return 1;
    }
    cout << "Dataset cargado correctamente. Total de tweets: " << tweets.size() << "\n\n";

    //Iniciar Experimento de Memoria
    cout << "[2/3] Ejecutando experimentos de Memoria..." << endl;
    string archivo_memoria = "memoria_consumida.csv";
    
    // Abrimos el archivo de memoria primero para escribirle la cabecera o limpiarla
    ofstream out_memoria(archivo_memoria);
    if (out_memoria.is_open()) {
        out_memoria << "estructura_de_datos;tipo_clave;memoria_usada_mb\n";
        out_memoria.close();
    } else {
        cerr << "Error al crear " << archivo_memoria << endl;
    }

    //Ejecutamos la memoria para ambas claves
    experimentoMemoria<long long>(tweets, archivo_memoria, "user_id");
    experimentoMemoria<string>(tweets, archivo_memoria, "user_screen_name");

    //Iniciar Experimentos de Tiempo
    cout << "[3/3] Ejecutando experimentos de Tiempo (20 repeticiones por estructura)..." << endl;


    //Ejecutamos los tiempos para la clave user_id
    cout << ">>> Iniciando pruebas de rendimiento para claves numericas (user_id) <<<" << endl;
    experimentoTiempos<long long>(tweets, "tiempos_user_id.csv", "auspol2019");

    //Ejecutamos los tiempos para la clave user_screen_name
    cout << ">>> Iniciando pruebas de rendimiento para claves de texto (user_screen_name) <<<" << endl;
    experimentoTiempos<string>(tweets, "tiempos_user_name.csv", "auspol2019");

    //Listo ╰(*°▽°*)╯
    cout << "========================================\n";
    cout << "   EXPERIMENTOS COMPLETADOS CON EXITO\n";
    cout << "========================================\n";
    cout << "Se han generado los siguientes archivos:\n";
    cout << " - tiempos_user_id.csv\n";
    cout << " - tiempos_user_name.csv\n";
    cout << " - memoria_consumida.csv\n";

    return 0;
}