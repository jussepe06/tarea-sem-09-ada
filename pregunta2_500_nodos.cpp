#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Arista {
    int peso;
    int u;
    int v;

    bool operator<(const Arista& otra) const {
        return peso < otra.peso;
    }
};

class Grafo {
public:
    int n;
    vector<Arista> aristas;

    Grafo(int n) : n(n) {}

    void agregar_arista(int u, int v, int peso) {
        aristas.push_back({peso, u, v});
    }

    vector<Arista> obtener_aristas() {
        return aristas;
    }
};

class UnionFind {
public:
    vector<int> parent;
    vector<int> rank;

    UnionFind(int tamano) {
        parent.resize(tamano);
        for (int i = 0; i < tamano; i++) parent[i] = i;
        rank.assign(tamano, 0);
    }

    int encontrar(int u) {
        if (parent[u] != u) {
            parent[u] = encontrar(parent[u]);
        }
        return parent[u];
    }

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

vector<Arista> Kruskal(Grafo G) {
    vector<Arista> MST;
    UnionFind uf(G.n);

    vector<Arista> aristas_ordenadas = G.obtener_aristas();
    sort(aristas_ordenadas.begin(), aristas_ordenadas.end());

    for (const auto& a : aristas_ordenadas) {
        if (uf.encontrar(a.u) != uf.encontrar(a.v)) {
            uf.unir(a.u, a.v);
            MST.push_back(a);
        }
    }
    return MST;
}

// Inserción dinámica de N nodos con pesos
Grafo generarGrafoDinamico(int numNodos, int factorAristas, mt19937& rng) {
    Grafo G(numNodos);
    uniform_int_distribution<int> distPeso(1, 100);

    // Conectar todos los nodos para asegurar conexidad
    for (int i = 1; i < numNodos; i++) {
        uniform_int_distribution<int> distPadre(0, i - 1);
        G.agregar_arista(i, distPadre(rng), distPeso(rng));
    }

    // Agregar aristas dinámicas adicionales
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

int main() {
    mt19937 rng(42);

    cout << "========================================================\n";
    cout << "  PREGUNTA 2: INSERCION DINAMICA DE 500 NODOS Y BENCHMARK \n";
    cout << "========================================================\n";

    // Prueba específica con 500 nodos
    Grafo grafo500 = generarGrafoDinamico(500, 4, rng);
    cout << "Grafo de 500 nodos generado:\n";
    cout << "- Nodos (V): " << grafo500.n << "\n";
    cout << "- Aristas (E): " << grafo500.aristas.size() << "\n";

    auto t1 = high_resolution_clock::now();
    vector<Arista> mst = Kruskal(grafo500);
    auto t2 = high_resolution_clock::now();
    auto tiempo500 = duration_cast<microseconds>(t2 - t1).count();

    cout << "- Aristas en MST: " << mst.size() << " (V - 1 = 499)\n";
    cout << "- Tiempo de ejecucion: " << tiempo500 << " microsegundos (" << tiempo500 / 1000.0 << " ms)\n\n";

    // Benchmarking con varios tamaños para analizar la complejidad Big-O
    cout << "Tabla de Benchmarking (de 50 a 500 nodos):\n";
    cout << "--------------------------------------------------------\n";
    cout << setw(10) << "Nodos (V)" << setw(14) << "Aristas (E)" << setw(16) << "Tiempo (us)" << setw(14) << "Tiempo (ms)\n";
    cout << "--------------------------------------------------------\n";

    vector<int> lista_n = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500};
    ofstream archivoCsv("benchmark_kruskal.csv");
    archivoCsv << "nodos,aristas,tiempo_us,tiempo_ms\n";

    for (int n : lista_n) {
        long long acum = 0;
        int e_count = 0;
        for (int rep = 0; rep < 10; rep++) {
            Grafo g = generarGrafoDinamico(n, 4, rng);
            e_count = g.aristas.size();
            auto inicio = high_resolution_clock::now();
            vector<Arista> res = Kruskal(g);
            auto fin = high_resolution_clock::now();
            acum += duration_cast<microseconds>(fin - inicio).count();
        }
        double promedio_us = acum / 10.0;
        cout << setw(10) << n << setw(14) << e_count << setw(16) << fixed << setprecision(2) << promedio_us << setw(14) << fixed << setprecision(4) << promedio_us / 1000.0 << "\n";
        archivoCsv << n << "," << e_count << "," << promedio_us << "," << promedio_us / 1000.0 << "\n";
    }
    archivoCsv.close();
    cout << "--------------------------------------------------------\n";

    return 0;
}
