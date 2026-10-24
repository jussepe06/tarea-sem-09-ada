#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Estructura para representar la arista: (peso, u, v)
struct Arista {
    int peso;
    int u;
    int v;

    // Para ordenar las aristas por peso de menor a mayor
    bool operator<(const Arista& otra) const {
        return peso < otra.peso;
    }
};

// ==========================================
// Clase Grafo (del pseudocódigo)
// ==========================================
class Grafo {
public:
    int n;                  // número de nodos
    vector<Arista> aristas; // lista de todas las aristas

    Grafo(int n) {
        this->n = n;
    }

    void agregar_arista(int u, int v, int peso) {
        aristas.push_back({peso, u, v});
    }

    vector<Arista> obtener_aristas() {
        return aristas;
    }
};

// ==========================================
// Clase UnionFind (del pseudocódigo)
// ==========================================
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

    int encontrar(int u) {
        if (parent[u] != u) {
            parent[u] = encontrar(parent[u]); // compresión de caminos
        }
        return parent[u];
    }

    void unir(int u, int v) {
        int raiz_u = encontrar(u);
        int raiz_v = encontrar(v);

        if (raiz_u != raiz_v) {
            // Unión por rango
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

// ==========================================
// Proceso Kruskal (del pseudocódigo)
// ==========================================
vector<Arista> Kruskal(Grafo G) {
    // Lista para guardar el MST
    vector<Arista> MST;

    // Crear una instancia de UnionFind
    UnionFind uf(G.n);

    // Paso 1: Ordenar todas las aristas en orden no decreciente de su peso
    vector<Arista> aristas_ordenadas = G.obtener_aristas();
    sort(aristas_ordenadas.begin(), aristas_ordenadas.end());

    // Paso 2: Agregar aristas al MST, asegurando que no formen ciclos
    for (const auto& arista : aristas_ordenadas) {
        int peso = arista.peso;
        int u = arista.u;
        int v = arista.v;

        if (uf.encontrar(u) != uf.encontrar(v)) {
            uf.unir(u, v);
            MST.push_back({peso, u, v});
        }
    }

    return MST;
}

// ==========================================
// Demostración de prueba
// ==========================================
int main() {
    // Grafo de ejemplo con 4 nodos (0, 1, 2, 3)
    Grafo g(4);
    g.agregar_arista(0, 1, 10);
    g.agregar_arista(0, 2, 6);
    g.agregar_arista(0, 3, 5);
    g.agregar_arista(1, 3, 15);
    g.agregar_arista(2, 3, 4);

    vector<Arista> mst = Kruskal(g);

    cout << "Aristas seleccionadas en el Arbol de Expansion Minima (MST):\n";
    int costoTotal = 0;
    for (const auto& a : mst) {
        cout << "Nodo " << a.u << " - Nodo " << a.v << " (Peso: " << a.peso << ")\n";
        costoTotal += a.peso;
    }
    cout << "Costo total del MST: " << costoTotal << endl;

    return 0;
}
