#include "parserdataset.hpp"
#include <fstream>
#include <iostream>

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
    bool primera_fila = true; // Para ignorar los encabezados

    // Leemos el archivo caracter por caracter
    while (archivo.get(c)) {
        if (c == '"') {
            entre_comillas = !entre_comillas; // Cambiamos el estado
        } 
        else if (c == ',' && !entre_comillas) {
            // Fin de una columna
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
            // Es parte del texto de la columna actual
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