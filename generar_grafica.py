import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Cargar los datos del benchmark generados por el programa C++
df = pd.read_csv('benchmark_kruskal.csv')

nodos = df['nodos'].values
aristas = df['aristas'].values
tiempo_us = df['tiempo_us'].values

# Modelo teórico O(E * log(E))
e_log_e = aristas * np.log2(aristas)
# Ajuste de escala para comparar en la misma gráfica
c = tiempo_us[-1] / e_log_e[-1]
tiempo_teorico = c * e_log_e

# Configuración visual de la gráfica
plt.figure(figsize=(10, 6), dpi=300)
plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')

# Graficar datos empíricos
plt.plot(nodos, tiempo_us, 'o-', color='#1f77b4', linewidth=2.5, markersize=7, label='Tiempo Real Medido (Benchmarking C++)')

# Graficar curva asintótica teórica Big-O
plt.plot(nodos, tiempo_teorico, '--', color='#d62728', linewidth=2, label='Curva Asintótica Teórica $O(E \\log E)$')

# Etiquetas y títulos
plt.title('Benchmarking y Análisis Asintótico Big-O: Algoritmo de Kruskal', fontsize=14, fontweight='bold', pad=15)
plt.xlabel('Número de Nodos ($|V|$)', fontsize=12, fontweight='bold')
plt.ylabel('Tiempo de Ejecución (Microsegundos - $\\mu s$)', fontsize=12, fontweight='bold')

# Anotaciones
plt.annotate(f'500 Nodos\n{tiempo_us[-1]:.1f} $\\mu s$', 
             xy=(nodos[-1], tiempo_us[-1]), 
             xytext=(nodos[-1]-60, tiempo_us[-1]+10),
             arrowprops=dict(facecolor='black', shrink=0.05, width=1, headwidth=6),
             fontweight='bold', color='#1f77b4')

plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=11, loc='upper left', frameon=True)
plt.tight_layout()

# Guardar la gráfica en formato PNG de alta resolución
output_path = 'grafica_benchmarking_kruskal.png'
plt.savefig(output_path, dpi=300)
print(f'Grafica guardada exitosamente en: {output_path}')
