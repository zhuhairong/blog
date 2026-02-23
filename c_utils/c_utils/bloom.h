#ifndef C_UTILS_BLOOM_H
#define C_UTILS_BLOOM_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct bloom_s bloom_t;

// 创建布隆过滤器
// n: 预期存储的元素数量
// p: 期望的假阳性概率 (例如 0.01 表示 1%)
// 返回: 成功返回过滤器指针，失败返回 NULL
bloom_t* bloom_create(size_t n, double p);

// 释放布隆过滤器
void     bloom_free(bloom_t *b);

// 添加元素
// b: 布隆过滤器
// data: 数据指针
// len: 数据长度
// 返回: 成功返回 true，失败返回 false
bool     bloom_add(bloom_t *b, const void *data, size_t len);

// 检查元素是否存在
// b: 布隆过滤器
// data: 数据指针
// len: 数据长度
// 返回: true 表示可能存在，false 表示肯定不存在
bool     bloom_check(const bloom_t *b, const void *data, size_t len);

// 重置布隆过滤器
// b: 布隆过滤器
// 返回: 成功返回 true，失败返回 false
bool     bloom_reset(bloom_t *b);

// 获取布隆过滤器统计信息
// b: 布隆过滤器
// estimated_elements: 估算的元素数量（输出参数）
// false_positive_rate: 当前假阳性概率（输出参数）
// 返回: 成功返回 true，失败返回 false
bool     bloom_stats(const bloom_t *b, size_t *estimated_elements, double *false_positive_rate);

// 序列化布隆过滤器
// b: 布隆过滤器
// buf: 输出缓冲区
// buf_size: 缓冲区大小
// written: 实际写入字节数（输出参数）
// 返回: 成功返回 true，失败返回 false
bool     bloom_serialize(const bloom_t *b, uint8_t *buf, size_t buf_size, size_t *written);

// 反序列化布隆过滤器
// buf: 输入缓冲区
// buf_size: 缓冲区大小
// 返回: 成功返回过滤器指针，失败返回 NULL
bloom_t* bloom_deserialize(const uint8_t *buf, size_t buf_size);

// 验证布隆过滤器输入参数
// n: 预期元素数量
// p: 假阳性概率
// 返回: 有效返回 true，无效返回 false
bool     bloom_validate_params(size_t n, double p);

#endif // C_UTILS_BLOOM_H
