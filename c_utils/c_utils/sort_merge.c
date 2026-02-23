#include "sort_merge.h"
#include <stdlib.h>
#include <string.h>

static void merge(int *arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int *L = malloc(n1 * sizeof(int)), *R = malloc(n2 * sizeof(int));
    memcpy(L, &arr[l], n1 * sizeof(int));
    memcpy(R, &arr[m + 1], n2 * sizeof(int));
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
}

static void merge_sort(int *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort(arr, l, m); merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void sort_merge(int *arr, size_t n) { if (n > 1) merge_sort(arr, 0, (int)n - 1); }
