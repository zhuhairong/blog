#ifndef C_UTILS_UUID_V7_H
#define C_UTILS_UUID_V7_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// UUID v7 错误码
typedef enum {
    UUID_V7_OK = 0,
    UUID_V7_INVALID_PARAMS = -1,
    UUID_V7_MEMORY_ERROR = -2,
    UUID_V7_GENERATION_ERROR = -3,
    UUID_V7_TIMESTAMP_ERROR = -4
} uuid_v7_error_t;

// UUID v7 配置选项
typedef struct {
    bool use_hyphens;          // 是否使用连字符
    bool uppercase;            // 是否使用大写字母
    bool use_monotonic;        // 是否使用单调递增
    bool use_entropy;          // 是否使用额外熵
    size_t entropy_size;       // 熵大小
} uuid_v7_config_t;

// UUID v7 上下文
typedef struct {
    uuid_v7_config_t config;
    uint64_t last_timestamp;   // 上次时间戳
    uint16_t sequence;         // 序列号
    uint64_t prng_state[2];    // PRNG 状态
    uuid_v7_error_t last_error;
} uuid_v7_ctx_t;

// 创建 UUID v7 上下文
uuid_v7_error_t uuid_v7_create(uuid_v7_ctx_t** ctx, const uuid_v7_config_t* config);

// 销毁 UUID v7 上下文
void uuid_v7_destroy(uuid_v7_ctx_t* ctx);

// 生成 UUID v7 (二进制格式)
// 返回 UUID_V7_OK 表示成功，其他值表示错误
uuid_v7_error_t uuid_v7_generate(uuid_v7_ctx_t* ctx, uint8_t *uuid);

// 生成 UUID v7 (字符串格式)
// 返回 UUID_V7_OK 表示成功，其他值表示错误
uuid_v7_error_t uuid_v7_generate_string(uuid_v7_ctx_t* ctx, char* out);

// 使用指定时间戳生成 UUID v7
// 返回 UUID_V7_OK 表示成功，其他值表示错误
uuid_v7_error_t uuid_v7_generate_with_timestamp(uuid_v7_ctx_t* ctx, uint64_t timestamp, uint8_t *uuid);

// 批量生成 UUID v7
// 返回 UUID_V7_OK 表示成功，其他值表示错误
uuid_v7_error_t uuid_v7_generate_batch(uuid_v7_ctx_t* ctx, uint8_t* uuids, size_t count);

// 从 UUID v7 中提取时间戳
// 返回 UUID_V7_OK 表示成功，其他值表示错误
uuid_v7_error_t uuid_v7_extract_timestamp(const uint8_t* uuid, uint64_t* timestamp);

// 获取最后一次错误
uuid_v7_error_t uuid_v7_get_last_error(uuid_v7_ctx_t* ctx);

// 获取错误信息
const char* uuid_v7_strerror(uuid_v7_error_t error);

#endif // C_UTILS_UUID_V7_H
