#include "sort.h"
#include <string.h>
#include <stdlib.h>

static void swap(void *a, void *b, size_t size) {
    char tmp[size];
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
}

void sort_quicksort(void *base, size_t nmemb, size_t size,
                   int (*compar)(const void *, const void *)) {
    if (nmemb < 2) return;

    char *pivot = (char*)base + (nmemb / 2) * size;
    char *l = (char*)base;
    char *r = (char*)base + (nmemb - 1) * size;

    while (l <= r) {
        while (compar(l, pivot) < 0) l += size;
        while (compar(r, pivot) > 0) r -= size;
        if (l <= r) {
            swap(l, r, size);
            // 如果交换的是 pivot 本身，需要更新 pivot 指针
            if (pivot == l) pivot = r;
            else if (pivot == r) pivot = l;
            l += size;
            r -= size;
        }
    }

    if ((size_t)(r - (char*)base) / size + 1 > 1)
        sort_quicksort(base, (r - (char*)base) / size + 1, size, compar);
    if (nmemb - ((l - (char*)base) / size) > 1)
        sort_quicksort(l, nmemb - ((l - (char*)base) / size), size, compar);
}
