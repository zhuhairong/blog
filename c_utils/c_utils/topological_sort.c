#include "topological_sort.h"

bool topological_sort(int n, int adj[10][10], int result[10]) {
    int in_degree[10] = {0};
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) if (adj[i][j]) in_degree[j]++;
    }
    int queue[10], head = 0, tail = 0, count = 0;
    for (int i = 0; i < n; i++) if (in_degree[i] == 0) queue[tail++] = i;
    while (head < tail) {
        int u = queue[head++];
        result[count++] = u;
        for (int v = 0; v < n; v++) {
            if (adj[u][v]) {
                if (--in_degree[v] == 0) queue[tail++] = v;
            }
        }
    }
    return count == n;
}
