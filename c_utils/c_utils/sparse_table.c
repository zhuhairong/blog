#include "sparse_table.h"
#include <stdlib.h>
#include <math.h>

void sparse_table_init(sparse_table_t *st, int *arr, size_t n) {
    st->n = n;
    st->max_log = (size_t)log2(n) + 1;
    st->st = malloc(n * sizeof(int*));
    for (size_t i = 0; i < n; i++) {
        st->st[i] = malloc(st->max_log * sizeof(int));
        st->st[i][0] = arr[i];
    }
    for (size_t j = 1; j < st->max_log; j++) {
        for (size_t i = 0; i + (1 << j) <= n; i++) {
            int a = st->st[i][j-1];
            int b = st->st[i + (1 << (j-1))][j-1];
            st->st[i][j] = (a < b) ? a : b;
        }
    }
}

int sparse_table_query(sparse_table_t *st, size_t l, size_t r) {
    size_t j = (size_t)log2(r - l + 1);
    int a = st->st[l][j];
    int b = st->st[r - (1 << j) + 1][j];
    return (a < b) ? a : b;
}

void sparse_table_free(sparse_table_t *st) {
    for (size_t i = 0; i < st->n; i++) free(st->st[i]);
    free(st->st);
}
