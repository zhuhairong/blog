#include "bloom_filter_counting.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 计数布隆过滤器结构
struct bloom_counting_s {
    uint8_t *counts;
    size_t size;
    int num_hashes;
    uint8_t max_count;
};

// MurmurHash3 简化版
static uint32_t murmurhash3(const void *key, size_t len, uint32_t seed) {
    const uint8_t *data = (const uint8_t*)key;
    uint32_t h = seed;
    for (size_t i = 0; i < len; i++) {
        h ^= data[i];
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

// 创建计数布隆过滤器
bloom_counting_t* bloom_counting_create(size_t size, int num_hashes, uint8_t max_count) {
    if (size == 0 || num_hashes == 0) return NULL;
    
    bloom_counting_t *bf = malloc(sizeof(bloom_counting_t));
    if (!bf) return NULL;
    
    bf->counts = calloc(size, sizeof(uint8_t));
    if (!bf->counts) {
        free(bf);
        return NULL;
    }
    
    bf->size = size;
    bf->num_hashes = num_hashes;
    bf->max_count = max_count > 0 ? max_count : 255;
    
    return bf;
}

// 释放计数布隆过滤器
void bloom_counting_free(bloom_counting_t *bf) {
    if (bf) {
        free(bf->counts);
        free(bf);
    }
}

// 添加元素
bool bloom_counting_add(bloom_counting_t *bf, const void *key, size_t key_len) {
    if (!bf || !key) return false;
    
    for (int i = 0; i < bf->num_hashes; i++) {
        uint32_t h = murmurhash3(key, key_len, (uint32_t)i) % bf->size;
        if (bf->counts[h] < bf->max_count) {
            bf->counts[h]++;
        }
    }
    return true;
}

// 移除元素
bool bloom_counting_remove(bloom_counting_t *bf, const void *key, size_t key_len) {
    if (!bf || !key) return false;
    
    for (int i = 0; i < bf->num_hashes; i++) {
        uint32_t h = murmurhash3(key, key_len, (uint32_t)i) % bf->size;
        if (bf->counts[h] > 0) {
            bf->counts[h]--;
        }
    }
    return true;
}

// 检查元素是否存在
bool bloom_counting_check(const bloom_counting_t *bf, const void *key, size_t key_len) {
    if (!bf || !key) return false;
    
    for (int i = 0; i < bf->num_hashes; i++) {
        uint32_t h = murmurhash3(key, key_len, (uint32_t)i) % bf->size;
        if (bf->counts[h] == 0) return false;
    }
    return true;
}

// 获取元素计数（估算）
uint8_t bloom_counting_estimate(const bloom_counting_t *bf, const void *key, size_t key_len) {
    if (!bf || !key) return 0;
    
    uint8_t min_count = bf->max_count;
    for (int i = 0; i < bf->num_hashes; i++) {
        uint32_t h = murmurhash3(key, key_len, (uint32_t)i) % bf->size;
        if (bf->counts[h] < min_count) {
            min_count = bf->counts[h];
        }
    }
    return min_count;
}

// 重置计数布隆过滤器
bool bloom_counting_reset(bloom_counting_t *bf) {
    if (!bf) return false;
    memset(bf->counts, 0, bf->size * sizeof(uint8_t));
    return true;
}

// 获取计数布隆过滤器统计信息
bool bloom_counting_stats(const bloom_counting_t *bf, size_t *total_elements, double *false_positive_rate) {
    if (!bf) return false;
    
    if (total_elements) {
        // 估算元素数量：所有计数器的平均值除以哈希函数数量
        size_t sum = 0;
        for (size_t i = 0; i < bf->size; i++) {
            sum += bf->counts[i];
        }
        *total_elements = sum / bf->num_hashes;
    }
    
    if (false_positive_rate) {
        // 假阳性概率估算
        double bits_set = 0;
        for (size_t i = 0; i < bf->size; i++) {
            if (bf->counts[i] > 0) bits_set++;
        }
        double p = bits_set / bf->size;
        *false_positive_rate = pow(p, bf->num_hashes);
    }
    
    return true;
}

// 序列化计数布隆过滤器
bool bloom_counting_serialize(const bloom_counting_t *bf, uint8_t *buf, size_t buf_size, size_t *written) {
    if (!bf || !buf) return false;
    
    size_t required = sizeof(size_t) + sizeof(int) + sizeof(uint8_t) + bf->size * sizeof(uint8_t);
    if (buf_size < required) return false;
    
    size_t offset = 0;
    memcpy(buf + offset, &bf->size, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(buf + offset, &bf->num_hashes, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, &bf->max_count, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(buf + offset, bf->counts, bf->size * sizeof(uint8_t));
    offset += bf->size * sizeof(uint8_t);
    
    if (written) *written = offset;
    return true;
}

// 反序列化计数布隆过滤器
bloom_counting_t* bloom_counting_deserialize(const uint8_t *buf, size_t buf_size) {
    if (!buf || buf_size < sizeof(size_t) + sizeof(int) + sizeof(uint8_t)) return NULL;
    
    size_t offset = 0;
    size_t size;
    int num_hashes;
    uint8_t max_count;
    
    memcpy(&size, buf + offset, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(&num_hashes, buf + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(&max_count, buf + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    
    if (buf_size < offset + size * sizeof(uint8_t)) return NULL;
    
    bloom_counting_t *bf = bloom_counting_create(size, num_hashes, max_count);
    if (!bf) return NULL;
    
    memcpy(bf->counts, buf + offset, size * sizeof(uint8_t));
    return bf;
}
