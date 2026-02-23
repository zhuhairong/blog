#include "binary_search.h"
#include <string.h>
#include <stdint.h>

static size_t get_index(const void *ptr, const void *base, size_t size) {
    return ((uintptr_t)ptr - (uintptr_t)base) / size;
}

static void* get_element(const void *base, size_t index, size_t size) {
    return (void*)((uintptr_t)base + index * size);
}

// 通用二分查找
void* binary_search(const void *key, const void *base, size_t nmemb, size_t size,
                   int (*compar)(const void *, const void *)) {
    if (!key || !base || !compar || nmemb == 0 || size == 0) {
        return NULL;
    }

    size_t low = 0, high = nmemb - 1;
    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_ptr = get_element(base, mid, size);
        int cmp = compar(key, mid_ptr);

        if (cmp == 0) {
            return (void*)mid_ptr;
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    return NULL;
}

// 二分查找（返回索引）
size_t binary_search_index(const void *key, const void *base, size_t nmemb, size_t size,
                         int (*compar)(const void *, const void *)) {
    void *ptr = binary_search(key, base, nmemb, size, compar);
    if (!ptr) {
        return (size_t)-1;
    }
    return get_index(ptr, base, size);
}

// 下界查找（返回第一个不小于 key 的元素指针）
void* binary_search_lower_bound(const void *key, const void *base, size_t nmemb, size_t size,
                              int (*compar)(const void *, const void *)) {
    if (!key || !base || !compar || nmemb == 0 || size == 0) {
        return NULL;
    }

    size_t low = 0, high = nmemb;
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_ptr = get_element(base, mid, size);
        int cmp = compar(key, mid_ptr);

        if (cmp <= 0) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    if (low < nmemb) {
        return (void*)get_element(base, low, size);
    }
    return NULL;
}

// 下界查找（返回索引）
size_t binary_search_lower_bound_index(const void *key, const void *base, size_t nmemb, size_t size,
                                     int (*compar)(const void *, const void *)) {
    void *ptr = binary_search_lower_bound(key, base, nmemb, size, compar);
    if (!ptr) {
        return nmemb; // 返回 nmemb 表示所有元素都小于 key
    }
    return get_index(ptr, base, size);
}

// 上界查找（返回第一个大于 key 的元素指针）
void* binary_search_upper_bound(const void *key, const void *base, size_t nmemb, size_t size,
                              int (*compar)(const void *, const void *)) {
    if (!key || !base || !compar || nmemb == 0 || size == 0) {
        return NULL;
    }

    size_t low = 0, high = nmemb;
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_ptr = get_element(base, mid, size);
        int cmp = compar(key, mid_ptr);

        if (cmp < 0) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    if (low < nmemb) {
        return (void*)get_element(base, low, size);
    }
    return NULL;
}

// 上界查找（返回索引）
size_t binary_search_upper_bound_index(const void *key, const void *base, size_t nmemb, size_t size,
                                     int (*compar)(const void *, const void *)) {
    void *ptr = binary_search_upper_bound(key, base, nmemb, size, compar);
    if (!ptr) {
        return nmemb; // 返回 nmemb 表示所有元素都小于等于 key
    }
    return get_index(ptr, base, size);
}

// 检查数组是否已排序
bool binary_search_is_sorted(const void *base, size_t nmemb, size_t size,
                           int (*compar)(const void *, const void *)) {
    if (!base || !compar || nmemb <= 1 || size == 0) {
        return true;
    }

    for (size_t i = 1; i < nmemb; i++) {
        const void *prev = get_element(base, i - 1, size);
        const void *curr = get_element(base, i, size);
        if (compar(prev, curr) > 0) {
            return false;
        }
    }

    return true;
}
