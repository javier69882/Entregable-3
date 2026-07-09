# Entregable 3: Estudio Experimental de Tablas Hash

Este repositorio contiene la implementación y el análisis de rendimiento de distintas estructuras de Tablas Hash (Hash Abierto y Hash Cerrado con diferentes estrategias de sondeo). El experimento procesa un dataset de 180.000 tweets para contar la cantidad de publicaciones por usuario.

## 1. Compilar y Ejecutar los Experimentos (C++)

El código base está preparado para compilarse en entornos **Linux o WSL (Windows Subsystem for Linux)**. 

Para ejecutar las pruebas y generar los archivos `.csv` con los resultados, abre tu terminal en la raíz del proyecto y utiliza los siguientes comandos:

1. **Compilar el proyecto:** *(Es obligatorio usar la bandera `-O3` para activar las optimizaciones de velocidad del procesador durante el experimento).*
   ```bash
   g++ -std=c++17 -O3 experimentos.cpp parserdataset.cpp -o experimentos.out
   ```

2. **Ejecutar las pruebas:**
   ```bash
   ./experimentos.out
   ```
   > **Nota:** Puede tardar un poco

---

##  2. Generar Gráficos (Python)

Una vez que el programa en C++ termine y genere los archivos `.csv` (`tiempos_user_id.csv`, `tiempos_user_name.csv` y `memoria_consumida.csv`), puedes generar los gráficos de escalabilidad y memoria.

Debido a las protecciones del sistema en distribuciones modernas de Linux (PEP 668), debes aislar las dependencias utilizando un **Entorno Virtual (`venv`)**.

Sigue estos pasos en tu terminal:

1. **Crear el entorno virtual:**
   ```bash
   python3 -m venv venv
   ```

2. **Activar el entorno virtual:**
   ```bash
   source venv/bin/activate
   ```
   *(Verás que tu terminal cambia y muestra `(venv)` al inicio de la línea).*

3. **Instalar las librerías requeridas:**
   ```bash
   pip install pandas matplotlib seaborn
   ```

4. **Generar las imágenes:**
   ```bash
   python3 generar_graficos.py
   ```
   Esto creará tres archivos `.png` en alta resolución con los resultados estadísticos listos para el informe.

> **Tip:** Para salir del entorno virtual de Python cuando termines, simplemente ejecuta el comando `deactivate` en la terminal.