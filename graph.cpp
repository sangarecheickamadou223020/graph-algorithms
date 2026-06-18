#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

class Graph {
private:
    int n; // Количество вершин
    vector<vector<int>> adjList; // Список смежности

public:
    Graph(int vertices) : n(vertices), adjList(vertices + 1) {}

    void addEdge(int u, int v, bool directed = false) {
        adjList[u].push_back(v);
        if (!directed && u != v) {
            adjList[v].push_back(u);
        }
    }

    // Сортировка соседей для лексикографического порядка
    void sortAdjLists() {
        for (auto& list : adjList) {
            sort(list.begin(), list.end());
        }
    }

    // ==================== ЧАСТЬ 1: Обходы ====================

    // BFS обход
    vector<int> bfs(int start) {
        vector<int> result;
        vector<bool> visited(n + 1, false);
        queue<int> q;

        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int current = q.front();
            q.pop();
            result.push_back(current);

            for (int neighbor : adjList[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }

        return result;
    }

    // DFS обход
    vector<int> dfs(int start) {
        vector<int> result;
        vector<bool> visited(n + 1, false);
        stack<int> st;

        st.push(start);
        visited[start] = true;

        while (!st.empty()) {
            int current = st.top();
            st.pop();
            result.push_back(current);

            // Добавляем соседей в порядке убывания для корректного порядка извлечения
            for (int i = adjList[current].size() - 1; i >= 0; i--) {
                int neighbor = adjList[current][i];
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    st.push(neighbor);
                }
            }
        }

        return result;
    }

    // ==================== ЧАСТЬ 2: Кратчайший путь ====================

    // BFS для кратчайшего пути
    pair<int, vector<int>> bfsShortestPath(int start, int end) {
        vector<int> parent(n + 1, -1);
        vector<bool> visited(n + 1, false);
        queue<int> q;

        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            if (current == end) break;

            for (int neighbor : adjList[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    parent[neighbor] = current;
                    q.push(neighbor);
                }
            }
        }

        // Восстановление пути
        vector<int> path;
        int current = end;

        if (parent[end] == -1 && start != end) {
            return {-1, path}; // Пути нет
        }

        while (current != -1) {
            path.push_back(current);
            current = parent[current];
        }

        reverse(path.begin(), path.end());
        int length = path.size() - 1;

        return {length, path};
    }

    // DFS для лексикографически первого пути
    bool dfsLexFirst(int current, int end, vector<bool>& visited,
                     vector<int>& path, vector<int>& result) {
        visited[current] = true;
        path.push_back(current);

        if (current == end) {
            result = path;
            return true;
        }

        for (int neighbor : adjList[current]) {
            if (!visited[neighbor]) {
                if (dfsLexFirst(neighbor, end, visited, path, result)) {
                    return true;
                }
            }
        }

        path.pop_back();
        return false;
    }

    vector<int> dfsLexFirstPath(int start, int end) {
        vector<bool> visited(n + 1, false);
        vector<int> path, result;

        dfsLexFirst(start, end, visited, path, result);

        return result;
    }

    // ==================== ЧАСТЬ 3: Компоненты связности ====================

    // Поиск компонент связности BFS
    vector<vector<int>> findComponentsBFS() {
        vector<vector<int>> components;
        vector<bool> visited(n + 1, false);

        for (int i = 1; i <= n; i++) {
            if (!visited[i]) {
                vector<int> component;
                queue<int> q;

                q.push(i);
                visited[i] = true;

                while (!q.empty()) {
                    int current = q.front();
                    q.pop();
                    component.push_back(current);

                    for (int neighbor : adjList[current]) {
                        if (!visited[neighbor]) {
                            visited[neighbor] = true;
                            q.push(neighbor);
                        }
                    }
                }

                sort(component.begin(), component.end());
                components.push_back(component);
            }
        }

        return components;
    }

    // Поиск компонент связности DFS
    void dfsComponentHelper(int current, vector<bool>& visited, vector<int>& component) {
        visited[current] = true;
        component.push_back(current);

        for (int neighbor : adjList[current]) {
            if (!visited[neighbor]) {
                dfsComponentHelper(neighbor, visited, component);
            }
        }
    }

    vector<vector<int>> findComponentsDFS() {
        vector<vector<int>> components;
        vector<bool> visited(n + 1, false);

        for (int i = 1; i <= n; i++) {
            if (!visited[i]) {
                vector<int> component;
                dfsComponentHelper(i, visited, component);
                sort(component.begin(), component.end());
                components.push_back(component);
            }
        }

        return components;
    }

    // Алгоритм Уоршелла для нахождения компонент связности
    vector<vector<int>> findComponentsFloydWarshall() {
        // Инициализация матрицы смежности
        vector<vector<bool>> adj(n + 1, vector<bool>(n + 1, false));

        for (int i = 1; i <= n; i++) {
            adj[i][i] = true;
            for (int j : adjList[i]) {
                adj[i][j] = true;
            }
        }

        // Алгоритм Уоршелла (модифицированный для неориентированных графов)
        for (int k = 1; k <= n; k++) {
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    if (adj[i][k] && adj[k][j]) {
                        adj[i][j] = true;
                    }
                }
            }
        }

        // Извлечение компонент из транзитивного замыкания
        vector<bool> processed(n + 1, false);
        vector<vector<int>> components;

        for (int i = 1; i <= n; i++) {
            if (!processed[i]) {
                vector<int> component;
                for (int j = 1; j <= n; j++) {
                    if (adj[i][j]) {
                        component.push_back(j);
                        processed[j] = true;
                    }
                }
                sort(component.begin(), component.end());
                components.push_back(component);
            }
        }

        return components;
    }

    int getVertexCount() const {
        return n;
    }
};

// ==================== Функции для тестирования ====================

Graph generateRandomGraph(int n, double probability) {
    Graph g(n);

    for (int i = 1; i <= n; i++) {
        for (int j = i + 1; j <= n; j++) {
            if ((double)rand() / RAND_MAX < probability) {
                g.addEdge(i, j);
            }
        }
    }

    g.sortAdjLists();
    return g;
}

void printVector(const vector<int>& vec, const string& label) {
    cout << label;
    for (int v : vec) {
        cout << v << " ";
    }
    cout << endl;
}

void printComponents(const vector<vector<int>>& components) {
    cout << "\nКомпоненты связности:" << endl;
    for (int i = 0; i < components.size(); i++) {
        cout << "- Компонента " << (i + 1) << ": ";
        for (int v : components[i]) {
            cout << v << " ";
        }
        cout << endl;
    }
    cout << "Всего компонент: " << components.size() << endl;
}

void testBasicExample() {
    cout << "==================== ПРИМЕР 1: Базовый граф ====================" << endl;

    Graph g(6);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);
    g.addEdge(3, 4);
    g.addEdge(5, 6);
    g.sortAdjLists();

    // BFS обход
    auto bfs_result = g.bfs(1);
    printVector(bfs_result, "Порядок обхода BFS (старт 1): ");

    // DFS обход
    auto dfs_result = g.dfs(1);
    printVector(dfs_result, "Порядок обхода DFS (старт 1): ");

    // Кратчайший путь
    auto [length, path] = g.bfsShortestPath(1, 4);
    cout << "Кратчайший путь (BFS) от 1 до 4: ";
    for (int i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i < path.size() - 1) cout << " → ";
    }
    cout << " (длина " << length << ")" << endl;

    // Лексикографически первый путь
    auto lex_path = g.dfsLexFirstPath(1, 4);
    cout << "Лексикографически первый путь (DFS) от 1 до 4: ";
    for (int i = 0; i < lex_path.size(); i++) {
        cout << lex_path[i];
        if (i < lex_path.size() - 1) cout << " → ";
    }
    cout << endl;

    // Компоненты связности
    auto components = g.findComponentsBFS();
    printComponents(components);
}

void performanceTest() {
    cout << "\n==================== ЧАСТЬ 3: Сравнение производительности ====================" << endl;

    vector<int> sizes = {50, 100, 200};
    vector<double> densities = {0.1, 0.3, 0.5};

    cout << fixed << setprecision(6);

    for (int n : sizes) {
        cout << "\n--- Граф размером n=" << n << " ---" << endl;
        cout << setw(10) << "Плотность" << setw(15) << "BFS (сек)" << setw(15) << "DFS (сек)"
             << setw(20) << "Уоршелл (сек)" << endl;
        cout << string(60, '-') << endl;

        for (double density : densities) {
            Graph g = generateRandomGraph(n, density);

            // BFS тест
            auto start_bfs = chrono::high_resolution_clock::now();
            auto comp_bfs = g.findComponentsBFS();
            auto end_bfs = chrono::high_resolution_clock::now();
            chrono::duration<double> time_bfs = end_bfs - start_bfs;

            // DFS тест
            auto start_dfs = chrono::high_resolution_clock::now();
            auto comp_dfs = g.findComponentsDFS();
            auto end_dfs = chrono::high_resolution_clock::now();
            chrono::duration<double> time_dfs = end_dfs - start_dfs;

            // Уоршелл тест
            auto start_fw = chrono::high_resolution_clock::now();
            auto comp_fw = g.findComponentsFloydWarshall();
            auto end_fw = chrono::high_resolution_clock::now();
            chrono::duration<double> time_fw = end_fw - start_fw;

            cout << setw(10) << density << setw(15) << time_bfs.count() << setw(15)
                 << time_dfs.count() << setw(20) << time_fw.count() << endl;
        }
    }
}

int main() {
    srand(time(0));

    // Тест базового примера
    testBasicExample();

    // Тест производительности
    performanceTest();

    return 0;
}
