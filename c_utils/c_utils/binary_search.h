#ifndef C_UTILS_BSEARCH_H
#define C_UTILS_BSEARCH_H

#include <stddef.h>
#include <stdbool.h>

// 通用二分查找
// 返回找到的元素指针，未找到返回 NULL
void* binary_search(const void *key, const void *base, size_t nmemb, size_t size,
                   int (*compar)(const void *, const void *));

// 二分查找（返回索引）
// 返回找到的元素索引，未找到返回 -1
size_t binary_search_index(const void *key, const void *base, size_t nmemb, size_t size,
                         int (*compar)(const void *, const void *));

// 下界查找（返回第一个不小于 key 的元素指针）
void* binary_search_lower_bound(const void *key, const void *base, size_t nmemb, size_t size,
                              int (*compar)(const void *, const void *));

// 下界查找（返回索引）
size_t binary_search_lower_bound_index(const void *key, const void *base, size_t nmemb, size_t size,
                                     int (*compar)(const void *, const void *));

// 上界查找（返回第一个大于 key 的元素指针）
void* binary_search_upper_bound(const void *key, const void *base, size_t nmemb, size_t size,
                              int (*compar)(const void *, const void *));

// 上界查找（返回索引）
size_t binary_search_upper_bound_index(const void *key, const void *base, size_t nmemb, size_t size,
                                     int (*compar)(const void *, const void *));

// 检查数组是否已排序
bool binary_search_is_sorted(const void *base, size_t nmemb, size_t size,
                           int (*compar)(const void *, const void *));

#endif // C_UTILS_BSEARCH_H
