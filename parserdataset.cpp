/**
 * @file parserdataset.cpp
 * @brief Implementación del parser para extraer el dataset de tweets desde un archivo CSV.
 * * Contiene la lógica para procesar archivos CSV de manera segura, manejando
 * correctamente los delimitadores y los campos encerrados entre comillas.
 */

#include "parserdataset.hpp"
#include <fstream>
#include <iostream>

/**
 * @brief Lee y procesa un archivo CSV para extraer identificadores y nombres de usuario.
 * * Implementa un analizador (parser) que lee el archivo carácter por carácter.
 * Esta aproximación permite distinguir entre las comas (',') que actúan como 
 * separadores de columnas y las comas que forman parte del contenido de texto, 
 * controlando un estado booleano cuando se encuentran comillas dobles ('"').
 * * Se extraen específicamente la columna 5 (user_id) y la columna 7 (user_screen_name).
 * Si una fila está corrupta o le faltan columnas, se ignora silenciosamente para 
 * no detener la carga masiva.
 * * @param rutaArchivo Ruta relativa o absoluta al archivo CSV a cargar.
 * @return std::vector<TweetData> Vector que contiene las estructuras de datos parseadas.
 * Retorna un vector vacío si el archivo no se pudo abrir.
 */
std::vector<TweetData> cargarDataset(const std::string& rutaArchivo) {
    std::vector<TweetData> dataset;
    std::ifstream archivo(rutaArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << rutaArchivo << std::endl;
        return dataset;
    }

    std::string campo_actual = "";
    std::vector<std::string> fila_actual;
    bool entre_comillas = false;
    char c;
    bool primera_fila = true; // Flag para ignorar la fila de encabezados

    // Leemos el archivo caracter por caracter
    while (archivo.get(c)) {
        if (c == '"') {
            entre_comillas = !entre_comillas; // Cambiamos el estado de lectura
        } 
        else if (c == ',' && !entre_comillas) {
            // Fin de una columna legítima
            fila_actual.push_back(campo_actual);
            campo_actual.clear();
        } 
        else if ((c == '\n' || c == '\r') && !entre_comillas) {
            // Fin de una fila
            if (c == '\r' && archivo.peek() == '\n') {
                archivo.get(); // Consumir el \n si es formato Windows (\r\n)
            }
            
            fila_actual.push_back(campo_actual);
            campo_actual.clear();

            // Si no es el encabezado y la fila tiene suficientes columnas
            if (!primera_fila && fila_actual.size() >= 8) {
                try {
                    TweetData datos;
                    // Índice 5 es user_id, Índice 7 es user_screen_name
                    datos.user_id = std::stoull(fila_actual[5]); 
                    datos.user_screen_name = fila_actual[7];
                    
                    dataset.push_back(datos);
                } catch (...) {
                    // Si falla la conversión (ej. fila corrupta o vacía), la ignoramos
                }
            }
            primera_fila = false;
            fila_actual.clear();
        } 
        else {
            // Es parte del texto de la columna actual, lo agregamos al buffer
            campo_actual += c;
        }
    }

    // Procesar la última fila si el archivo no termina en un salto de línea
    if (!fila_actual.empty() || !campo_actual.empty()) {
        fila_actual.push_back(campo_actual);
        if (!primera_fila && fila_actual.size() >= 8) {
            try {
                TweetData datos;
                datos.user_id = std::stoull(fila_actual[5]);
                datos.user_screen_name = fila_actual[7];
                dataset.push_back(datos);
            } catch (...) {}
        }
    }

    archivo.close();
    return dataset;
}