#ifndef C_UTILS_BLOOM_COUNTING_H
#define C_UTILS_BLOOM_COUNTING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct bloom_counting_s bloom_counting_t;

// 创建计数布隆过滤器
// size: 过滤器大小（位数）
// num_hashes: 哈希函数数量
// max_count: 每个计数器的最大值（建议 15 或 255）
// 返回: 成功返回过滤器指针，失败返回 NULL
bloom_counting_t* bloom_counting_create(size_t size, int num_hashes, uint8_t max_count);

// 释放计数布隆过滤器
void bloom_counting_free(bloom_counting_t *bf);

// 添加元素
// bf: 计数布隆过滤器
// key: 键指针
// key_len: 键长度
// 返回: 成功返回 true，失败返回 false
bool bloom_counting_add(bloom_counting_t *bf, const void *key, size_t key_len);

// 移除元素
// bf: 计数布隆过滤器
// key: 键指针
// key_len: 键长度
// 返回: 成功返回 true，失败返回 false
bool bloom_counting_remove(bloom_counting_t *bf, const void *key, size_t key_len);

// 检查元素是否存在
// bf: 计数布隆过滤器
// key: 键指针
// key_len: 键长度
// 返回: true 表示可能存在，false 表示肯定不存在
bool bloom_counting_check(const bloom_counting_t *bf, const void *key, size_t key_len);

// 获取元素计数（估算）
// bf: 计数布隆过滤器
// key: 键指针
// key_len: 键长度
// 返回: 估算的计数，失败返回 0
uint8_t bloom_counting_estimate(const bloom_counting_t *bf, const void *key, size_t key_len);

// 重置计数布隆过滤器
// bf: 计数布隆过滤器
// 返回: 成功返回 true，失败返回 false
bool bloom_counting_reset(bloom_counting_t *bf);

// 获取计数布隆过滤器统计信息
// bf: 计数布隆过滤器
// total_elements: 估算的总元素数量（输出参数）
// false_positive_rate: 当前假阳性概率（输出参数）
// 返回: 成功返回 true，失败返回 false
bool bloom_counting_stats(const bloom_counting_t *bf, size_t *total_elements, double *false_positive_rate);

// 序列化计数布隆过滤器
// bf: 计数布隆过滤器
// buf: 输出缓冲区
// buf_size: 缓冲区大小
// written: 实际写入字节数（输出参数）
// 返回: 成功返回 true，失败返回 false
bool bloom_counting_serialize(const bloom_counting_t *bf, uint8_t *buf, size_t buf_size, size_t *written);

// 反序列化计数布隆过滤器
// buf: 输入缓冲区
// buf_size: 缓冲区大小
// 返回: 成功返回过滤器指针，失败返回 NULL
bloom_counting_t* bloom_counting_deserialize(const uint8_t *buf, size_t buf_size);

// 验证计数布隆过滤器输入参数
// size: 过滤器大小
// num_hashes: 哈希函数数量
// max_count: 最大计数值
// 返回: 有效返回 true，无效返回 false
bool bloom_counting_validate_params(size_t size, int num_hashes, uint8_t max_count);

#endif // C_UTILS_BLOOM_COUNTING_H
