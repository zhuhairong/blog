#ifndef C_UTILS_DISJOINT_SET_FOREST_H
#define C_UTILS_DISJOINT_SET_FOREST_H

#include <stddef.h>
#include <stdbool.h>

// 并查集错误码
typedef enum {
    DSF_OK = 0,
    DSF_ERROR_INVALID_PARAM,
    DSF_ERROR_INVALID_ELEMENT,
    DSF_ERROR_MEMORY_ALLOC,
    DSF_ERROR_EMPTY_SET
} dsf_error_t;

// 并查集
typedef struct {
    int *parent;
    int *rank;
    size_t *size;
    size_t n;
    size_t sets;
    bool has_error;
    dsf_error_t error;
    char error_msg[256];
} dsf_t;

// 创建并查集
// n: 元素数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回并查集指针，失败返回 NULL
dsf_t* dsf_create(size_t n, dsf_error_t *error);

// 初始化并查集
// dsf: 并查集
// n: 元素数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dsf_init(dsf_t *dsf, size_t n, dsf_error_t *error);

// 释放并查集
// dsf: 并查集
void dsf_free(dsf_t *dsf);

// 查找元素的根
// dsf: 并查集
// i: 元素索引
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回根元素索引，失败返回 -1
int dsf_find(dsf_t *dsf, int i, dsf_error_t *error);

// 合并两个集合
// dsf: 并查集
// i: 第一个元素
// j: 第二个元素
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dsf_union(dsf_t *dsf, int i, int j, dsf_error_t *error);

// 检查两个元素是否在同一集合
// dsf: 并查集
// i: 第一个元素
// j: 第二个元素
// error: 错误码（输出参数，可为 NULL）
// 返回: 在同一集合返回 true，否则返回 false
bool dsf_same_set(dsf_t *dsf, int i, int j, dsf_error_t *error);

// 获取集合数量
// dsf: 并查集
// 返回: 集合数量
size_t dsf_get_set_count(const dsf_t *dsf);

// 获取集合大小
// dsf: 并查集
// i: 元素索引
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回集合大小，失败返回 0
size_t dsf_get_set_size(const dsf_t *dsf, int i, dsf_error_t *error);

// 批量合并
// dsf: 并查集
// pairs: 元素对数组 [i1, j1, i2, j2, ...]
// count: 元素对数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dsf_batch_union(dsf_t *dsf, const int *pairs, size_t count, dsf_error_t *error);

// 重置并查集
// dsf: 并查集
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dsf_reset(dsf_t *dsf, dsf_error_t *error);

// 检查并查集是否有错误
// dsf: 并查集
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool dsf_has_error(const dsf_t *dsf, dsf_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* dsf_strerror(dsf_error_t error);

#endif // C_UTILS_DISJOINT_SET_FOREST_H
