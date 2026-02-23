#include "kruskal.h"
#include "disjoint_set_forest.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

static int compare(const void *a, const void *b) {
    return ((kruskal_edge_t*)a)->w - ((kruskal_edge_t*)b)->w;
}

static bool validate_input(int n, int m, kruskal_edge_t *edges, kruskal_error_t *error) {
    if (!edges) {
        if (error) *error = KRUSKAL_INVALID_INPUT;
        return false;
    }

    if (n <= 0) {
        if (error) *error = KRUSKAL_VERTEX_ERROR;
        return false;
    }

    if (m < n - 1) {
        if (error) *error = KRUSKAL_DISCONNECTED;
        return false;
    }

    for (int i = 0; i < m; i++) {
        if (edges[i].u < 0 || edges[i].u >= n || edges[i].v < 0 || edges[i].v >= n) {
            if (error) *error = KRUSKAL_EDGE_ERROR;
            return false;
        }
    }

    return true;
}

int kruskal_mst(int n, int m, kruskal_edge_t *edges) {
    kruskal_error_t error;
    int result = kruskal_mst_ex(n, m, edges, &error);
    return (error == KRUSKAL_OK) ? result : -1;
}

int kruskal_mst_ex(int n, int m, kruskal_edge_t *edges, kruskal_error_t *error) {
    if (!validate_input(n, m, edges, error)) {
        return -1;
    }

    qsort(edges, m, sizeof(kruskal_edge_t), compare);

    dsf_t dsf;
    dsf_error_t dsf_err;
    if (!dsf_init(&dsf, (size_t)n, &dsf_err)) {
        if (error) *error = KRUSKAL_MEMORY_ERROR;
        return -1;
    }

    int total_weight = 0, edges_count = 0;
    for (int i = 0; i < m && edges_count < n - 1; i++) {
        if (dsf_find(&dsf, edges[i].u, NULL) != dsf_find(&dsf, edges[i].v, NULL)) {
            dsf_union(&dsf, edges[i].u, edges[i].v, NULL);
            total_weight += edges[i].w;
            edges_count++;
        }
    }

    dsf_free(&dsf);

    if (edges_count != n - 1) {
        if (error) *error = KRUSKAL_DISCONNECTED;
        return -1;
    }

    if (error) *error = KRUSKAL_OK;
    return total_weight;
}

kruskal_result_t *kruskal_mst_with_config(int n, int m, kruskal_edge_t *edges, 
                                          const kruskal_config_t *config, 
                                          kruskal_error_t *error) {
    if (!config) {
        if (error) *error = KRUSKAL_INVALID_INPUT;
        return NULL;
    }

    if (config->enable_validation && !validate_input(n, m, edges, error)) {
        return NULL;
    }

    kruskal_result_t *result = (kruskal_result_t *)malloc(sizeof(kruskal_result_t));
    if (!result) {
        if (error) *error = KRUSKAL_MEMORY_ERROR;
        return NULL;
    }

    result->total_weight = 0;
    result->edges = NULL;
    result->edge_count = 0;
    result->has_error = false;
    result->error = KRUSKAL_OK;
    memset(result->error_msg, 0, sizeof(result->error_msg));

    if (config->return_edges) {
        result->edges = (kruskal_edge_t *)malloc(config->max_edges * sizeof(kruskal_edge_t));
        if (!result->edges) {
            free(result);
            if (error) *error = KRUSKAL_MEMORY_ERROR;
            return NULL;
        }
    }

    qsort(edges, m, sizeof(kruskal_edge_t), compare);

    dsf_t dsf;
    dsf_error_t dsf_err;
    if (!dsf_init(&dsf, (size_t)n, &dsf_err)) {
        if (result->edges) free(result->edges);
        free(result);
        if (error) *error = KRUSKAL_MEMORY_ERROR;
        return NULL;
    }

    int edges_count = 0;
    for (int i = 0; i < m && edges_count < n - 1; i++) {
        if (dsf_find(&dsf, edges[i].u, NULL) != dsf_find(&dsf, edges[i].v, NULL)) {
            dsf_union(&dsf, edges[i].u, edges[i].v, NULL);
            result->total_weight += edges[i].w;
            
            if (config->return_edges && edges_count < (int)config->max_edges) {
                result->edges[edges_count] = edges[i];
            }
            
            edges_count++;
        }
    }

    dsf_free(&dsf);

    if (edges_count != n - 1) {
        result->has_error = true;
        result->error = KRUSKAL_DISCONNECTED;
        snprintf(result->error_msg, sizeof(result->error_msg), "Graph is disconnected");
        if (error) *error = KRUSKAL_DISCONNECTED;
        return result;
    }

    result->edge_count = (size_t)edges_count;
    
    if (error) *error = KRUSKAL_OK;
    return result;
}

void kruskal_free_result(kruskal_result_t *result) {
    if (!result) return;
    
    if (result->edges) {
        free(result->edges);
        result->edges = NULL;
    }
    
    free(result);
}

void kruskal_get_default_config(kruskal_config_t *config) {
    if (!config) return;
    
    config->enable_validation = true;
    config->return_edges = false;
    config->max_edges = 0;
}
