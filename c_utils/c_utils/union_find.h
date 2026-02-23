#ifndef C_UTILS_UNION_FIND_H
#define C_UTILS_UNION_FIND_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    int *parent;
    int *rank;
    size_t n;
} union_find_t;

union_find_t* uf_create(size_t n);
void          uf_free(union_find_t *uf);
int           uf_find(union_find_t *uf, int i);
void          uf_union(union_find_t *uf, int i, int j);
bool          uf_connected(union_find_t *uf, int i, int j);

#endif // C_UTILS_UNION_FIND_H
