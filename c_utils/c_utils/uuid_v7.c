#include "uuid_v7.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// xorshift128+ PRNG
static uint64_t xorshift128plus(uint64_t *state) {
    uint64_t x = state[0];
    uint64_t y = state[1];
    state[0] = y;
    x ^= x << 23;
    state[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
    return state[1] + y;
}

// 获取当前时间戳（毫秒）
static uint64_t get_current_timestamp_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
}

uuid_v7_error_t uuid_v7_create(uuid_v7_ctx_t** ctx, const uuid_v7_config_t* config) {
    if (!ctx) return UUID_V7_INVALID_PARAMS;
    
    *ctx = malloc(sizeof(uuid_v7_ctx_t));
    if (!*ctx) return UUID_V7_MEMORY_ERROR;
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.use_hyphens = true;
        (*ctx)->config.uppercase = false;
        (*ctx)->config.use_monotonic = true;
        (*ctx)->config.use_entropy = false;
        (*ctx)->config.entropy_size = 0;
    }
    
    (*ctx)->last_timestamp = 0;
    (*ctx)->sequence = 0;
    (*ctx)->prng_state[0] = (uint64_t)rand() << 32 | rand();
    (*ctx)->prng_state[1] = (uint64_t)rand() << 32 | rand();
    (*ctx)->last_error = UUID_V7_OK;
    
    return UUID_V7_OK;
}

void uuid_v7_destroy(uuid_v7_ctx_t* ctx) {
    free(ctx);
}

uuid_v7_error_t uuid_v7_generate(uuid_v7_ctx_t* ctx, uint8_t *uuid) {
    if (!ctx || !uuid) return UUID_V7_INVALID_PARAMS;
    
    uint64_t timestamp = get_current_timestamp_ms();
    return uuid_v7_generate_with_timestamp(ctx, timestamp, uuid);
}

uuid_v7_error_t uuid_v7_generate_string(uuid_v7_ctx_t* ctx, char* out) {
    if (!ctx || !out) return UUID_V7_INVALID_PARAMS;
    
    uint8_t uuid[16];
    uuid_v7_error_t err = uuid_v7_generate(ctx, uuid);
    if (err != UUID_V7_OK) return err;
    
    const char *fmt = ctx->config.uppercase ? 
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X" :
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x";
    
    sprintf(out, fmt,
        uuid[0], uuid[1], uuid[2], uuid[3],
        uuid[4], uuid[5], uuid[6], uuid[7],
        uuid[8], uuid[9], uuid[10], uuid[11],
        uuid[12], uuid[13], uuid[14], uuid[15]);
    
    return UUID_V7_OK;
}

uuid_v7_error_t uuid_v7_generate_with_timestamp(uuid_v7_ctx_t* ctx, uint64_t timestamp, uint8_t *uuid) {
    if (!ctx || !uuid) return UUID_V7_INVALID_PARAMS;
    
    // 处理单调递增
    if (ctx->config.use_monotonic) {
        if (timestamp <= ctx->last_timestamp) {
            ctx->sequence++;
            if (ctx->sequence == 0) {
                // 序列号溢出，增加时间戳
                ctx->last_timestamp++;
            }
        } else {
            ctx->last_timestamp = timestamp;
            ctx->sequence = 0;
        }
        timestamp = ctx->last_timestamp;
    }
    
    // 填充时间戳（48位，大端序）
    uuid[0] = (timestamp >> 40) & 0xFF;
    uuid[1] = (timestamp >> 32) & 0xFF;
    uuid[2] = (timestamp >> 24) & 0xFF;
    uuid[3] = (timestamp >> 16) & 0xFF;
    uuid[4] = (timestamp >> 8) & 0xFF;
    uuid[5] = timestamp & 0xFF;
    
    // 版本和随机数据
    uint64_t r1 = xorshift128plus(ctx->prng_state);
    uint64_t r2 = xorshift128plus(ctx->prng_state);
    
    // 版本 7 (4位)
    uuid[6] = 0x70 | ((r1 >> 8) & 0x0F);
    uuid[7] = r1 & 0xFF;
    
    // 变体 1 (2位) 和随机数据
    uuid[8] = 0x80 | ((r2 >> 8) & 0x3F);
    uuid[9] = r2 & 0xFF;
    
    // 剩余随机数据
    uuid[10] = (r2 >> 56) & 0xFF;
    uuid[11] = (r2 >> 48) & 0xFF;
    uuid[12] = (r2 >> 40) & 0xFF;
    uuid[13] = (r2 >> 32) & 0xFF;
    uuid[14] = (r2 >> 24) & 0xFF;
    uuid[15] = (r2 >> 16) & 0xFF;
    
    // 如果使用单调递增，将序列号嵌入随机部分
    if (ctx->config.use_monotonic && ctx->sequence > 0) {
        uuid[10] = (ctx->sequence >> 8) & 0xFF;
        uuid[11] = ctx->sequence & 0xFF;
    }
    
    return UUID_V7_OK;
}

uuid_v7_error_t uuid_v7_generate_batch(uuid_v7_ctx_t* ctx, uint8_t* uuids, size_t count) {
    if (!ctx || !uuids || count == 0) return UUID_V7_INVALID_PARAMS;
    
    for (size_t i = 0; i < count; i++) {
        uuid_v7_error_t err = uuid_v7_generate(ctx, uuids + i * 16);
        if (err != UUID_V7_OK) return err;
    }
    
    return UUID_V7_OK;
}

uuid_v7_error_t uuid_v7_extract_timestamp(const uint8_t* uuid, uint64_t* timestamp) {
    if (!uuid || !timestamp) return UUID_V7_INVALID_PARAMS;
    
    *timestamp = ((uint64_t)uuid[0] << 40) |
                 ((uint64_t)uuid[1] << 32) |
                 ((uint64_t)uuid[2] << 24) |
                 ((uint64_t)uuid[3] << 16) |
                 ((uint64_t)uuid[4] << 8) |
                 (uint64_t)uuid[5];
    
    return UUID_V7_OK;
}

uuid_v7_error_t uuid_v7_get_last_error(uuid_v7_ctx_t* ctx) {
    return ctx ? ctx->last_error : UUID_V7_INVALID_PARAMS;
}

const char* uuid_v7_strerror(uuid_v7_error_t error) {
    switch (error) {
        case UUID_V7_OK: return "Success";
        case UUID_V7_INVALID_PARAMS: return "Invalid parameters";
        case UUID_V7_MEMORY_ERROR: return "Memory error";
        case UUID_V7_GENERATION_ERROR: return "Generation error";
        case UUID_V7_TIMESTAMP_ERROR: return "Timestamp error";
        default: return "Unknown error";
    }
}
