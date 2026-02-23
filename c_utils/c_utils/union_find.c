#include "union_find.h"
#include <stdlib.h>
#include <stdbool.h>

union_find_t* uf_create(size_t n) {
    if (n == 0) return NULL;
    
    union_find_t *uf = malloc(sizeof(union_find_t));
    if (!uf) return NULL;
    
    uf->parent = malloc(n * sizeof(int));
    if (!uf->parent) {
        free(uf);
        return NULL;
    }
    
    uf->rank = calloc(n, sizeof(int));
    if (!uf->rank) {
        free(uf->parent);
        free(uf);
        return NULL;
    }
    
    uf->n = n;
    for (size_t i = 0; i < n; i++) uf->parent[i] = (int)i;
    return uf;
}

int uf_find(union_find_t *uf, int i) {
    if (!uf || i < 0 || (size_t)i >= uf->n) return -1;
    if (uf->parent[i] == i) return i;
    return uf->parent[i] = uf_find(uf, uf->parent[i]);
}

void uf_union(union_find_t *uf, int i, int j) {
    if (!uf) return;
    if (i < 0 || (size_t)i >= uf->n || j < 0 || (size_t)j >= uf->n) return;
    
    int root_i = uf_find(uf, i);
    int root_j = uf_find(uf, j);
    if (root_i == -1 || root_j == -1) return;
    
    if (root_i != root_j) {
        if (uf->rank[root_i] < uf->rank[root_j]) uf->parent[root_i] = root_j;
        else if (uf->rank[root_i] > uf->rank[root_j]) uf->parent[root_j] = root_i;
        else { uf->parent[root_i] = root_j; uf->rank[root_j]++; }
    }
}

bool uf_connected(union_find_t *uf, int i, int j) {
    if (!uf) return false;
    int root_i = uf_find(uf, i);
    int root_j = uf_find(uf, j);
    if (root_i == -1 || root_j == -1) return false;
    return root_i == root_j;
}

void uf_free(union_find_t *uf) {
    if (uf) {
        free(uf->parent);
        free(uf->rank);
        free(uf);
    }
}
