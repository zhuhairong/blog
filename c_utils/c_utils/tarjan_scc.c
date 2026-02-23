#include "tarjan_scc.h"
#include <stdbool.h>

static int disc[TARJAN_MAX_NODES], low[TARJAN_MAX_NODES], stack[TARJAN_MAX_NODES];
static bool on_stack[TARJAN_MAX_NODES];
static int timer, top, scc_count;

static void find_scc(int u, tarjan_graph_t *g, int *scc_map) {
    disc[u] = low[u] = ++timer;
    stack[++top] = u;
    on_stack[u] = true;
    for (int i = 0; i < g->adj_size[u]; i++) {
        int v = g->adj[u][i];
        if (disc[v] == -1) {
            find_scc(v, g, scc_map);
            low[u] = (low[u] < low[v]) ? low[u] : low[v];
        } else if (on_stack[v]) {
            low[u] = (low[u] < disc[v]) ? low[u] : disc[v];
        }
    }
    if (low[u] == disc[u]) {
        while (true) {
            int v = stack[top--];
            on_stack[v] = false;
            scc_map[v] = scc_count;
            if (u == v) break;
        }
        scc_count++;
    }
}

int tarjan_scc(tarjan_graph_t *g, int *scc_map) {
    timer = top = scc_count = 0;
    for (int i = 0; i < g->n; i++) { disc[i] = -1; on_stack[i] = false; }
    for (int i = 0; i < g->n; i++) { if (disc[i] == -1) find_scc(i, g, scc_map); }
    return scc_count;
}
