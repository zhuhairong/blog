#include "sort_utils.h"
#include <stdlib.h>
#include <string.h>

static void swap(char *a, char *b, size_t size) {
    char tmp;
    while (size--) {
        tmp = *a;
        *a++ = *b;
        *b++ = tmp;
    }
}

void sort_utils_quicksort(void *base, size_t nmemb, size_t size, 
                    int (*compar)(const void *, const void *)) {
    if (nmemb < 2) return;
    
    char *pivot = (char *)base + (nmemb / 2) * size;
    char *left = (char *)base;
    char *right = (char *)base + (nmemb - 1) * size;

    while (left <= right) {
        while (compar(left, pivot) < 0) left += size;
        while (compar(right, pivot) > 0) right -= size;
        if (left <= right) {
            swap(left, right, size);
            // 如果 pivot 被交换了，更新它的位置
            if (pivot == left) pivot = right;
            else if (pivot == right) pivot = left;
            left += size;
            right -= size;
        }
    }

    if ((size_t)(right - (char *)base) / size + 1 > 1)
        sort_utils_quicksort(base, (right - (char *)base) / size + 1, size, compar);
    if (nmemb - ((left - (char *)base) / size) > 1)
        sort_utils_quicksort(left, nmemb - ((left - (char *)base) / size), size, compar);
}

int sort_binary_search(const void *key, const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *)) {
    int low = 0;
    int high = (int)nmemb - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int res = compar(key, (char *)base + mid * size);
        if (res == 0) return mid;
        if (res < 0) high = mid - 1;
        else low = mid + 1;
    }
    return -1;
}
