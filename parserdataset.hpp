#ifndef PARSERDATASET_HPP
#define PARSERDATASET_HPP

/**
 * @file parserdataset.hpp
 * @brief Definición de las estructuras y funciones para la lectura del dataset.
 * * Provee la interfaz para extraer y almacenar en memoria los datos relevantes 
 * desde un archivo CSV.
 */

#include <string>
#include <vector>

/**
 * @brief Estructura ligera para almacenar exclusivamente los datos necesarios del CSV.
 * * Se ignoran las columnas irrelevantes del dataset original para optimizar 
 * el consumo de memoria durante los experimentos.
 */
struct TweetData {
    long long user_id;              ///< Identificador numérico único del usuario (Columna 5).
    std::string user_screen_name;   ///< Nombre de usuario o handle de texto (Columna 7).
};

/**
 * @brief Lee y procesa un archivo CSV para extraer el dataset.
 * * Maneja internamente el parseo carácter por carácter para evitar errores 
 * con delimitadores (comas) que se encuentren dentro del texto de los tweets.
 * * @param rutaArchivo Ruta al archivo CSV que contiene los datos.
 * @return std::vector<TweetData> Vector con todos los registros parseados exitosamente.
 */
std::vector<TweetData> cargarDataset(const std::string& rutaArchivo);

#endif 