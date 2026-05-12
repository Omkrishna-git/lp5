#include <bits/stdc++.h>
#include <omp.h>
#include <chrono>

using namespace std;

class Graph {
public:
    int vertices, edges;
    vector<vector<int>> graph;
    vector<bool> visited;

    Graph() {
        cout << "Enter number of vertices: ";
        cin >> vertices;

        cout << "Enter number of edges: ";
        cin >> edges;

        graph.resize(vertices);
        visited.resize(vertices, false);

        cout << "Enter edges (u v):\n";
        for (int i = 0; i < edges; i++) {
            int u, v;
            cin >> u >> v;
            addEdge(u, v);
        }
    }

    void addEdge(int u, int v) {
        graph[u].push_back(v);
        graph[v].push_back(u); // undirected
    }

    void resetVisited() {
        fill(visited.begin(), visited.end(), false);
    }

    void printGraph() {
        cout << "\nAdjacency List:\n";
        for (int i = 0; i < vertices; i++) {
            cout << i << " -> ";
            for (int j : graph[i]) {
                cout << j << " ";
            }
            cout << endl;
        }
    }

    // ---------------- SEQUENTIAL DFS ----------------
    void dfs(int start) {
        stack<int> s;
        s.push(start);
        visited[start] = true;

        while (!s.empty()) {
            int current = s.top();
            s.pop();

            cout << current << " ";

            for (int neighbor : graph[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    s.push(neighbor);
                }
            }
        }
    }

    // ---------------- PARALLEL DFS ----------------
    void parallel_dfs(int start) {
        stack<int> s;
        s.push(start);
        visited[start] = true;

        while (!s.empty()) {
            int current;

            #pragma omp critical
            {
                if (!s.empty()) {
                    current = s.top();
                    s.pop();
                    cout << current << " ";
                }
            }

            #pragma omp parallel for
            for (int i = 0; i < graph[current].size(); i++) {
                int neighbor = graph[current][i];

                if (!visited[neighbor]) {
                    #pragma omp critical
                    {
                        if (!visited[neighbor]) {
                            visited[neighbor] = true;
                            s.push(neighbor);
                        }
                    }
                }
            }
        }
    }

    // ---------------- SEQUENTIAL BFS ----------------
    void bfs(int start) {
        queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            cout << current << " ";

            for (int neighbor : graph[current]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
    }

    // ---------------- PARALLEL BFS ----------------
    void parallel_bfs(int start) {
        vector<int> frontier;
        frontier.push_back(start);
        visited[start] = true;

        while (!frontier.empty()) {
            vector<int> next_frontier;

            #pragma omp parallel
            {
                vector<int> local_next;

                #pragma omp for nowait
                for (int i = 0; i < frontier.size(); i++) {
                    int current = frontier[i];

                    cout << current << " ";

                    for (int neighbor : graph[current]) {
                        if (!visited[neighbor]) {
                            #pragma omp critical
                            {
                                if (!visited[neighbor]) {
                                    visited[neighbor] = true;
                                    local_next.push_back(neighbor);
                                }
                            }
                        }
                    }
                }

                #pragma omp critical
                next_frontier.insert(next_frontier.end(),
                                     local_next.begin(),
                                     local_next.end());
            }

            frontier = next_frontier;
        }
    }
};

// ---------------- MAIN MENU ----------------
int main() {
    Graph g;
    g.printGraph();

    int choice, start;

    do {
        cout << "\n===== MENU =====\n";
        cout << "1. DFS\n";
        cout << "2. Parallel DFS\n";
        cout << "3. BFS\n";
        cout << "4. Parallel BFS\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice >= 1 && choice <= 4) {
            cout << "Enter starting node: ";
            cin >> start;

            g.resetVisited();

            auto start_time = chrono::high_resolution_clock::now();
 
            switch (choice) {
                case 1:
                    cout << "DFS: ";
                    g.dfs(start);
                    break;

                case 2:
                    cout << "Parallel DFS: ";
                    g.parallel_dfs(start);
                    break;

                case 3:
                    cout << "BFS: ";
                    g.bfs(start);
                    break;

                case 4:
                    cout << "Parallel BFS: ";
                    g.parallel_bfs(start);
                    break;
            }

            auto end_time = chrono::high_resolution_clock::now();

            cout << "\nTime taken: "
                 << chrono::duration_cast<chrono::microseconds>(
                        end_time - start_time)
                        .count()
                 << " microseconds\n";
        }

    } while (choice != 5);

    return 0;
}


// Enter number of vertices: 6
// Enter number of edges: 5
// Enter edges (u v):
// 0 1
// 1 2
// 1 3
// 2 4
// 2 5


// Enter number of vertices: 7
// Enter number of edges: 4
// Enter edges (u v):
// 0 1
// 1 2
// 3 4
// 5 6
