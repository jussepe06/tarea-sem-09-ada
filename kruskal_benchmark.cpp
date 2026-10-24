#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Estructura de arista que representa (peso, u, v) exactamente como en el pseudocódigo
struct Arista {
    int peso;
    int u;
    int v;

    // Sobrecarga del operador < para ordenar aristas por peso de forma no decreciente
    bool operator<(const Arista& otra) const {
        return peso < otra.peso;
    }
};

// -------------------------------------------------------------
// 1. Clase Grafo basada en el pseudocódigo
// -------------------------------------------------------------
class Grafo {
public:
    int n;                   // número de nodos
    vector<Arista> aristas;  // lista de todas las aristas

    Grafo(int n) : n(n) {}

    // función agregar_arista(self, u, v, peso)
    void agregar_arista(int u, int v, int peso) {
        aristas.push_back({peso, u, v});
    }

    // función obtener_aristas(self)
    vector<Arista>& obtener_aristas() {
        return aristas;
    }
};

// -------------------------------------------------------------
// 2. Clase UnionFind basada en el pseudocódigo
// -------------------------------------------------------------
class UnionFind {
public:
    vector<int> parent; // padre de cada elemento
    vector<int> rank;   // rango de cada elemento para optimización

    UnionFind(int tamano) {
        parent.resize(tamano);
        for (int i = 0; i < tamano; i++) {
            parent[i] = i;
        }
        rank.assign(tamano, 0);
    }

    // función encontrar(self, u) con compresión de caminos
    int encontrar(int u) {
        if (parent[u] != u) {
            parent[u] = encontrar(parent[u]); // compresión de caminos
        }
        return parent[u];
    }

    // función unir(self, u, v) con unión por rango
    void unir(int u, int v) {
        int raiz_u = encontrar(u);
        int raiz_v = encontrar(v);
        if (raiz_u != raiz_v) {
            if (rank[raiz_u] > rank[raiz_v]) {
                parent[raiz_v] = raiz_u;
            } else if (rank[raiz_u] < rank[raiz_v]) {
                parent[raiz_u] = raiz_v;
            } else {
                parent[raiz_v] = raiz_u;
                rank[raiz_u] += 1;
            }
        }
    }
};

// -------------------------------------------------------------
// 3. Proceso Kruskal basado en el pseudocódigo
// -------------------------------------------------------------
vector<Arista> kruskal(Grafo& G) {
    // Lista para guardar el MST
    vector<Arista> MST;
    // Crear una instancia de UnionFind
    UnionFind uf(G.n);

    // Paso 1: Ordenar todas las aristas en orden no decreciente de su peso
    vector<Arista> aristas_ordenadas = G.obtener_aristas();
    sort(aristas_ordenadas.begin(), aristas_ordenadas.end());

    // Paso 2: Agregar aristas al MST, asegurando que no formen ciclos
    for (const auto& a : aristas_ordenadas) {
        if (uf.encontrar(a.u) != uf.encontrar(a.v)) {
            uf.unir(a.u, a.v);
            MST.push_back(a);
        }
    }

    return MST;
}

// -------------------------------------------------------------
// Función auxiliar: Generación dinámica de grafo conectado
// -------------------------------------------------------------
Grafo generarGrafoDinamico(int numNodos, int factorAristas, mt19937& rng) {
    Grafo G(numNodos);
    uniform_int_distribution<int> distPeso(1, 100);

    // Garantizar que el grafo sea conexo (árbol generador inicial)
    for (int i = 1; i < numNodos; i++) {
        uniform_int_distribution<int> distPadre(0, i - 1);
        int u = i;
        int v = distPadre(rng);
        int peso = distPeso(rng);
        G.agregar_arista(u, v, peso);
    }

    // Agregar aristas adicionales para densidad dinámica
    int aristasExtra = (numNodos * factorAristas) - (numNodos - 1);
    uniform_int_distribution<int> distNodo(0, numNodos - 1);
    for (int i = 0; i < aristasExtra; i++) {
        int u = distNodo(rng);
        int v = distNodo(rng);
        if (u != v) {
            G.agregar_arista(u, v, distPeso(rng));
        }
    }

    return G;
}

// -------------------------------------------------------------
// Función Principal con Demostración y Benchmarking
// -------------------------------------------------------------
int main() {
    mt19937 rng(42); // Semilla fija para reproducibilidad científica

    cout << "======================================================================\n";
    cout << "     ALGORITMO DE KRUSKAL (MST) Y BENCHMARKING ASINTOTICO BIG-O       \n";
    cout << "======================================================================\n\n";

    // -------------------------------------------------------------
    // Parte 1: Demostración con grafo de 500 nodos solicitados
    // -------------------------------------------------------------
    cout << "[1] Generando de forma dinamica un grafo de 500 nodos...\n";
    Grafo grafo500 = generarGrafoDinamico(500, 4, rng);
    cout << "    - Numero de Nodos (|V|): " << grafo500.n << "\n";
    cout << "    - Numero de Aristas (|E|): " << grafo500.aristas.size() << "\n";

    auto t_inicio = high_resolution_clock::now();
    vector<Arista> mst500 = kruskal(grafo500);
    auto t_fin = high_resolution_clock::now();
    auto duracion_us = duration_cast<microseconds>(t_fin - t_inicio).count();

    long long costoTotal = 0;
    for (const auto& a : mst500) {
        costoTotal += a.peso;
    }

    cout << "    - Aristas seleccionadas en el MST: " << mst500.size() << " (Esperado: |V|-1 = 499)\n";
    cout << "    - Costo total del MST: " << costoTotal << "\n";
    cout << "    - Tiempo de ejecucion para 500 nodos: " << duracion_us << " us (" << (duracion_us / 1000.0) << " ms)\n\n";

    // -------------------------------------------------------------
    // Parte 2: Benchmarking con multiples tamanos para analisis Big-O
    // -------------------------------------------------------------
    cout << "======================================================================\n";
    cout << "[2] BENCHMARKING DE TIEMPOS DE EJECUCION (Varying V de 50 a 500)\n";
    cout << "======================================================================\n";
    cout << setw(10) << "Nodos (V)" 
         << setw(14) << "Aristas (E)" 
         << setw(16) << "Tiempo (us)" 
         << setw(16) << "Tiempo (ms)" << "\n";
    cout << "----------------------------------------------------------------------\n";

    vector<int> tamanios = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500};
    ofstream csv("benchmark_kruskal.csv");
    csv << "nodos,aristas,tiempo_us,tiempo_ms\n";

    for (int n : tamanios) {
        const int REPETICIONES = 10;
        long long suma_us = 0;
        int aristas_count = 0;

        for (int rep = 0; rep < REPETICIONES; rep++) {
            Grafo g = generarGrafoDinamico(n, 4, rng);
            aristas_count = g.aristas.size();

            auto start = high_resolution_clock::now();
            vector<Arista> res = kruskal(g);
            auto stop = high_resolution_clock::now();

            suma_us += duration_cast<microseconds>(stop - start).count();
        }

        double tiempo_promedio_us = (double)suma_us / REPETICIONES;
        double tiempo_promedio_ms = tiempo_promedio_us / 1000.0;

        cout << setw(10) << n 
             << setw(14) << aristas_count 
             << setw(16) << fixed << setprecision(2) << tiempo_promedio_us 
             << setw(16) << fixed << setprecision(4) << tiempo_promedio_ms << "\n";

        csv << n << "," << aristas_count << "," << tiempo_promedio_us << "," << tiempo_promedio_ms << "\n";
    }

    csv.close();
    cout << "----------------------------------------------------------------------\n";
    cout << "Resultados exportados con exito a: benchmark_kruskal.csv\n";
    cout << "======================================================================\n";

    return 0;
}
