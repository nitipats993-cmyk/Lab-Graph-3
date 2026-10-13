#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

// Node structure
class Node {
public:
    char name;
    Node(char n) : name(n) {}
};

// Edge structure
class Edge {
public:
    char u;
    char v;
    int weight;
    Edge(char _u, char _v, int _w) : u(_u), v(_v), weight(_w) {}
};

// Graph structure
class Graph {
    private:
    vector<Node> nodes;
    vector<Edge> edges;
    vector<vector<int>> matrix;
    vector<char> node_labels;

    // Helper for Kruskal's Algorithm (Disjoint Set)
    char find_set(char i, vector<char>& parent) {
        if (parent[i] == i)
            return i;
        return parent[i] = find_set(parent[i], parent);
    }

    void union_sets(char i, char j, vector<char>& parent, vector<int>& rank) {
        char root_i = find_set(i, parent);
        char root_j = find_set(j, parent);
        if (root_i != root_j) {
            if (rank[root_i] < rank[root_j]) {
                parent[root_i] = root_j;
            } else if (rank[root_i] > rank[root_j]) {
                parent[root_j] = root_i;
            } else {
                parent[root_j] = root_i;
                rank[root_i]++;
            }
        }
    }

public:
    Graph(const vector<vector<int>>& adj_matrix, const vector<char>& labels) {
        matrix = adj_matrix;
        node_labels = labels;

        for (char label : labels) {
            nodes.push_back(Node(label));
        }

        int n = labels.size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                int weight = adj_matrix[i][j];
                if (weight > 0) {
                    edges.push_back(Edge(labels[i], labels[j], weight));
                }
            }
        }
    }

    // --- Lab 1: Check graph properties ---
    bool is_multigraph() {
        // Multigraph allows multiple edges between the same pair of nodes, but no self-loops
        for (const auto& edge : edges) {
            if (edge.u == edge.v) return false; 
        }
        
        // Check if any pair of nodes has more than 1 edge
        int n = node_labels.size();
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                char u = node_labels[i];
                char v = node_labels[j];
                int count = 0;
                for (const auto& edge : edges) {
                    if ((edge.u == u && edge.v == v) || (edge.u == v && edge.v == u)) {
                        count++;
                    }
                }
                if (count > 1) return true;
            }
        }
        return false;
    }

    bool is_pseudograph() {
        for (const auto& edge : edges) {
            if (edge.u == edge.v) return true; 
        }
        return is_multigraph();
    }

    bool is_digraph() {
        int n = node_labels.size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (matrix[i][j] != matrix[j][i]) return true;
            }
        }
        return false;
    }

    bool is_weighted_graph() {
        for (const auto& edge : edges) {
            if (edge.weight != 1 && edge.weight != 0) return true;
        }
        return !edges.empty();
    }

    bool is_complete_graph() {
        int n = node_labels.size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j && matrix[i][j] == 0) return false;
            }
        }
        return true;
    }

    bool is_disjoint_graph() {
        for (char label : node_labels) {
            int degree = 0;
            for (const auto& edge : edges) {
                if (edge.u == label || edge.v == label) degree++;
            }
            if (degree == 0) return true;
        }
        return false;
    }

    pair<vector<Edge>, int> get_minimum_spanning_tree() {
        vector<Edge> unique_edges;
        // Filter out duplicate edges (since it's undirected)
        for (const auto& edge : edges) {
            if (edge.u < edge.v) {
                unique_edges.push_back(edge);
            }
        }

        // Sort edges by weight in ascending order
        sort(unique_edges.begin(), unique_edges.end(), [](const Edge& a, const Edge& b) {
            return a.weight < b.weight;
        });

        vector<char> parent;
        vector<int> rank;
        char max_label = 'I';
        parent.resize(max_label + 1);
        rank.resize(max_label + 1, 0);

        for (char label : node_labels) {
            parent[label] = label;
        }

        vector<Edge> mst;
        int total_weight = 0;

        for (const auto& edge : unique_edges) {
            char root_u = find_set(edge.u, parent);
            char root_v = find_set(edge.v, parent);

            if (root_u != root_v) {
                union_sets(root_u, root_v, parent, rank);
                mst.push_back(edge);
                total_weight += edge.weight;
            }
        }

        return {mst, total_weight};
    }

    void dijkstra(char start_node) {
        struct NodeDist {
            char node;
            int dist;
            bool operator>(const NodeDist& other) const {
                return dist > other.dist;
            }
        };

        priority_queue<NodeDist, vector<NodeDist>, greater<NodeDist>> pq;

        auto get_index = [this](char c) { return c - 'A'; };
        vector<int> distances(256, 1e9);

        distances[start_node] = 0;
        pq.push({start_node, 0});

        while (!pq.empty()) {
            NodeDist current = pq.top();
            pq.pop();

            char u = current.node;
            int d = current.dist;

            if (d > distances[u]) continue;

            int u_idx = get_index(u);
            for (size_t i = 0; i < node_labels.size(); ++i) {
                int weight = matrix[u_idx][i];
                if (weight > 0) {
                    char v = node_labels[i];
                    if (distances[u] + weight < distances[v]) {
                        distances[v] = distances[u] + weight;
                        pq.push({v, distances[v]});
                    }
                }
            }
        }

        cout << "\n=== Result for GRAPH #3 (Dijkstra's Shortest Path from '" << start_node << "') ===" << endl;
        for (char label : node_labels) {
            cout << "Shortest distance from " << start_node << " to " << label << " = " << distances[label] << endl;
        }
    }
};

int main() {
    // Sample adjacency matrix data from Graph #1
    vector<char> labels = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
    vector<vector<int>> sample_matrix = {
        {0, 2, 1, 7, 0, 0, 0, 0, 0},
        {2, 0, 5, 5, 0, 0, 0, 0, 0},
        {1, 5, 0, 4, 0, 9, 0, 0, 0},
        {7, 5, 4, 0, 8, 0, 0, 0, 0},
        {0, 0, 0, 8, 0, 3, 7, 0, 0},
        {0, 0, 9, 0, 3, 0, 5, 10, 0},
        {0, 0, 0, 0, 7, 5, 0, 11, 6},
        {0, 0, 0, 0, 0, 10, 11, 0, 3},
        {0, 0, 0, 0, 0, 0, 6, 3, 0}
    };

    Graph g(sample_matrix, labels);

    // Lab 1 result
    cout << "=== Result for GRAPH #1 (Properties) ===" << endl;
    cout << "Is it a multigraph?      : " << (g.is_multigraph() ? "True" : "False") << endl;
    cout << "Is it a pseudograph?     : " << (g.is_pseudograph() ? "True" : "False") << endl;
    cout << "Is it a digraph?         : " << (g.is_digraph() ? "True" : "False") << endl;
    cout << "Is it a weighted graph?  : " << (g.is_weighted_graph() ? "True" : "False") << endl;
    cout << "Is it a complete graph?  : " << (g.is_complete_graph() ? "True" : "False") << endl;
    cout << "Is it a disjoint graph?  : " << (g.is_disjoint_graph() ? "True" : "False") << endl;

    // Lab 2 result
    cout << "\n=== Result for GRAPH #2 (Minimum Spanning Tree) ===" << endl;
    auto [mst_edges, mst_total] = g.get_minimum_spanning_tree();
    cout << "Total Weight of MST: " << mst_total << endl;
    cout << "Edges in MST:" << endl;
    for (const auto& edge : mst_edges) {
        cout << "  " << edge.u << " - " << edge.v << " (Weight: " << edge.weight << ")" << endl;
    }

    // Lab 3 result
    g.dijkstra('A');

    return 0;
}