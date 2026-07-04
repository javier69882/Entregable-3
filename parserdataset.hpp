#ifndef PARSERDATASET_HPP
#define PARSERDATASET_HPP

#include <string>
#include <vector>

// Estructura para almacenar solo los datos que necesitamos del CSV
struct TweetData {
    long long user_id;
    std::string user_screen_name;
};

// Función para cargar el dataset desde el archivo CSV
std::vector<TweetData> cargarDataset(const std::string& rutaArchivo);

#endif 