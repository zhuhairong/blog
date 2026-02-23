#ifndef C_UTILS_UUID_V4_FAST_H
#define C_UTILS_UUID_V4_FAST_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// UUID v4 快速生成错误码
typedef enum {
    UUID_V4_FAST_OK = 0,
    UUID_V4_FAST_INVALID_PARAMS = -1,
    UUID_V4_FAST_MEMORY_ERROR = -2,
    UUID_V4_FAST_GENERATION_ERROR = -3
} uuid_v4_fast_error_t;

// UUID v4 快速生成配置选项
typedef struct {
    bool use_hyphens;          // 是否使用连字符
    bool uppercase;            // 是否使用大写字母
    bool use_entropy_pool;     // 是否使用熵池
    size_t prng_seed_size;     // PRNG 种子大小
} uuid_v4_fast_config_t;

// UUID v4 快速生成上下文
typedef struct {
    uuid_v4_fast_config_t config;
    uint64_t prng_state[2];    // PRNG 状态
    uuid_v4_fast_error_t last_error;
    uint64_t generate_count;   // 生成计数
} uuid_v4_fast_ctx_t;

// 创建 UUID v4 快速生成上下文
uuid_v4_fast_error_t uuid_v4_fast_create(uuid_v4_fast_ctx_t** ctx, const uuid_v4_fast_config_t* config);

// 销毁 UUID v4 快速生成上下文
void uuid_v4_fast_destroy(uuid_v4_fast_ctx_t* ctx);

// 快速生成 UUID v4 (二进制格式)
// 返回 UUID_V4_FAST_OK 表示成功，其他值表示错误
uuid_v4_fast_error_t uuid_v4_fast_generate(uuid_v4_fast_ctx_t* ctx, uint8_t *uuid);

// 快速生成 UUID v4 (字符串格式)
// 返回 UUID_V4_FAST_OK 表示成功，其他值表示错误
uuid_v4_fast_error_t uuid_v4_fast_generate_string(uuid_v4_fast_ctx_t* ctx, char* out);

// 批量生成 UUID v4
// 返回 UUID_V4_FAST_OK 表示成功，其他值表示错误
uuid_v4_fast_error_t uuid_v4_fast_generate_batch(uuid_v4_fast_ctx_t* ctx, uint8_t* uuids, size_t count);

// 重置 PRNG 种子
// 返回 UUID_V4_FAST_OK 表示成功，其他值表示错误
uuid_v4_fast_error_t uuid_v4_fast_reset_seed(uuid_v4_fast_ctx_t* ctx, const uint8_t* seed, size_t seed_len);

// 获取生成计数
uint64_t uuid_v4_fast_get_generate_count(uuid_v4_fast_ctx_t* ctx);

// 获取最后一次错误
uuid_v4_fast_error_t uuid_v4_fast_get_last_error(uuid_v4_fast_ctx_t* ctx);

// 获取错误信息
const char* uuid_v4_fast_strerror(uuid_v4_fast_error_t error);

#endif // C_UTILS_UUID_V4_FAST_H
