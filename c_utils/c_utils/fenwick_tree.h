#ifndef C_UTILS_FENWICK_TREE_H
#define C_UTILS_FENWICK_TREE_H

#include <stddef.h>
#include <stdbool.h>

// Fenwick Tree 错误码
typedef enum {
    FENWICK_OK = 0,
    FENWICK_ERROR_INVALID_PARAM,
    FENWICK_ERROR_INVALID_INDEX,
    FENWICK_ERROR_MEMORY_ALLOC,
    FENWICK_ERROR_EMPTY_TREE,
    FENWICK_ERROR_OUT_OF_BOUNDS
} fenwick_error_t;

// Fenwick Tree 数据类型
typedef enum {
    FENWICK_TYPE_INT,
    FENWICK_TYPE_LONG,
    FENWICK_TYPE_FLOAT,
    FENWICK_TYPE_DOUBLE
} fenwick_type_t;

// Fenwick Tree
typedef struct {
    void *tree;
    size_t n;
    size_t capacity;
    fenwick_type_t type;
    bool has_error;
    fenwick_error_t error;
    char error_msg[256];
} fenwick_tree_t;

// 创建 Fenwick Tree
// n: 大小
// type: 数据类型
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 Fenwick Tree 指针，失败返回 NULL
fenwick_tree_t* fenwick_tree_create(size_t n, fenwick_type_t type, fenwick_error_t *error);

// 释放 Fenwick Tree
// ft: Fenwick Tree
void fenwick_tree_free(fenwick_tree_t *ft);

// 初始化 Fenwick Tree
// ft: Fenwick Tree
// n: 大小
// type: 数据类型
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fenwick_tree_init(fenwick_tree_t *ft, size_t n, fenwick_type_t type, fenwick_error_t *error);

// 更新 Fenwick Tree
// ft: Fenwick Tree
// idx: 索引（1-based）
// val: 值
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fenwick_tree_update(fenwick_tree_t *ft, size_t idx, double val, fenwick_error_t *error);

// 查询前缀和
// ft: Fenwick Tree
// idx: 索引（1-based）
// result: 结果（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fenwick_tree_query(fenwick_tree_t *ft, size_t idx, double *result, fenwick_error_t *error);

// 查询范围和
// ft: Fenwick Tree
// l: 左索引（1-based）
// r: 右索引（1-based）
// result: 结果（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fenwick_tree_range_query(fenwick_tree_t *ft, size_t l, size_t r, double *result, fenwick_error_t *error);

// 批量更新
// ft: Fenwick Tree
// indices: 索引数组（1-based）
// values: 值数组
// count: 数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fenwick_tree_batch_update(fenwick_tree_t *ft, const size_t *indices, const double *values, size_t count, fenwick_error_t *error);

// 批量查询
// ft: Fenwick Tree
// indices: 索引数组（1-based）
// results: 结果数组（输出参数）
// count: 数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fenwick_tree_batch_query(fenwick_tree_t *ft, const size_t *indices, double *results, size_t count, fenwick_error_t *error);

// 获取 Fenwick Tree 大小
// ft: Fenwick Tree
// 返回: 大小
size_t fenwick_tree_size(const fenwick_tree_t *ft);

// 检查 Fenwick Tree 是否为空
// ft: Fenwick Tree
// 返回: 为空返回 true，否则返回 false
bool fenwick_tree_is_empty(const fenwick_tree_t *ft);

// 检查 Fenwick Tree 是否有错误
// ft: Fenwick Tree
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool fenwick_tree_has_error(const fenwick_tree_t *ft, fenwick_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* fenwick_tree_strerror(fenwick_error_t error);

#endif // C_UTILS_FENWICK_TREE_H
