"""
Script de visualización de datos para experimentos de rendimiento de Tablas Hash.

Este módulo lee los archivos CSV generados por las pruebas en C++ y utiliza 
Pandas, Matplotlib y Seaborn para renderizar gráficos de escalabilidad (tiempo) 
y consumo de memoria RAM.
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Configuración visual
sns.set_theme(style="whitegrid")
plt.rcParams.update({'figure.max_open_warning': 0, 'font.size': 12})

"""
    Genera un gráfico de líneas para evaluar la escalabilidad en tiempo de ejecución.

    Lee los tiempos acumulados desde un archivo CSV y grafica el crecimiento 
    del tiempo (en milisegundos) a medida que aumentan las inserciones. Incluye 
    un sombreado que representa la desviación estándar de los datos y una caja 
    de texto con las métricas finales.

    Args:
        csv_file (str): Ruta al archivo CSV con los datos de tiempo (ej. 'tiempos_user_id.csv').
        titulo (str): Subtítulo descriptivo que se añadirá al título principal del gráfico.
        output_name (str): Nombre del archivo de salida para guardar la imagen (ej. 'grafico.png').

    Returns:
        None: Guarda la imagen en el disco duro y no retorna ningún valor.
    """
def plot_escalabilidad(csv_file, titulo, output_name):
    if not os.path.exists(csv_file):
        print(f"Archivo {csv_file} no encontrado.")
        return

    # Leer el CSV
    df = pd.read_csv(csv_file, sep=';')
    
    # Convertir a milisegundos
    df['tiempo_ejecucion_ms'] = df['tiempo_ejecucion_us'] / 1000.0

    # Ampliamos un poco el ancho (de 10 a 11) para que quepa la leyenda afuera
    fig, ax = plt.subplots(figsize=(11, 6))
    
    # errorbar='sd' fuerza a que la sombra sea exactamente la Desviación Estándar
    sns.lineplot(
        data=df, 
        x='cantidad_consultas', 
        y='tiempo_ejecucion_ms', 
        hue='estructura_de_datos', 
        marker='o',
        linewidth=2,
        errorbar='sd', 
        ax=ax
    )

    ax.set_title(f'Rendimiento de Escalabilidad - {titulo}', fontsize=16, fontweight='bold')
    ax.set_xlabel('Cantidad de Tweets Insertados', fontsize=14)
    ax.set_ylabel('Tiempo Acumulado Promedio (ms)', fontsize=14)
    
    # Calcular la desviación estándar final para escribirla en pantalla
    max_consultas = df['cantidad_consultas'].max()
    df_final = df[df['cantidad_consultas'] == max_consultas]
    stats = df_final.groupby('estructura_de_datos')['tiempo_ejecucion_ms'].std().reset_index()
    
    # Construir el texto que irá en la caja
    texto_caja = f"Desviación Est. (a los {max_consultas} tweets):\n"
    for _, row in stats.iterrows():
        texto_caja += f"• {row['estructura_de_datos']}: ±{row['tiempo_ejecucion_ms']:.2f} ms\n"
    texto_caja += "\n* La sombra en las curvas también\n  representa la desviación estándar."
    
    # Agregar la caja de texto al gráfico en la esquina superior izquierda
    props = dict(boxstyle='round', facecolor='white', alpha=0.9, edgecolor='gray')
    ax.text(0.02, 0.96, texto_caja, transform=ax.transAxes, fontsize=10,
            verticalalignment='top', bbox=props)

    # Mover la leyenda afuera a la derecha para que no estorbe con las líneas ni la caja
    ax.legend(title='Estructura de Datos', bbox_to_anchor=(1.02, 1), loc='upper left')
    plt.tight_layout()
    
    # Guardar
    plt.savefig(output_name, dpi=300)
    print(f"Gráfico guardado: {output_name}")
    plt.close()

def plot_memoria(csv_file, output_name):
    """
    Genera un gráfico de barras para comparar el consumo de memoria RAM.

    Evalúa el *peak* de memoria utilizado por cada estructura de datos,
    agrupando los resultados por el tipo de clave utilizada (numérica o texto).

    Args:
        csv_file (str): Ruta al archivo CSV con los datos de memoria.
        output_name (str): Nombre del archivo de salida para la imagen generada.

    Returns:
        None: Guarda la imagen generada en el disco duro.
    """
    if not os.path.exists(csv_file):
        print(f"Archivo {csv_file} no encontrado.")
        return

    df = pd.read_csv(csv_file, sep=';')

    plt.figure(figsize=(10, 6))
    
    sns.barplot(
        data=df, 
        x='estructura_de_datos', 
        y='memoria_usada_mb', 
        hue='tipo_clave',
        palette='viridis'
    )

    plt.title('Consumo de Memoria por Estructura y Tipo de Clave', fontsize=16, fontweight='bold')
    plt.xlabel('Estructura de Datos', fontsize=14)
    plt.ylabel('Memoria RAM Usada (MB)', fontsize=14)
    plt.xticks(rotation=15)
    plt.legend(title='Tipo de Clave')
    plt.tight_layout()
    
    plt.savefig(output_name, dpi=300)
    print(f"Gráfico guardado: {output_name}")
    plt.close()

if __name__ == '__main__':
    print("Generando gráficos actualizados con promedios y desviaciones...")
    
    # Ejecución de los procesos de visualización
    plot_escalabilidad('tiempos_user_id.csv', 'Claves Numéricas (user_id)', 'grafico_tiempos_id.png')
    plot_escalabilidad('tiempos_user_name.csv', 'Claves de Texto (user_screen_name)', 'grafico_tiempos_nombre.png')
    plot_memoria('memoria_consumida.csv', 'grafico_memoria.png')
    
    print("¡Todos los gráficos han sido generados con éxito!")