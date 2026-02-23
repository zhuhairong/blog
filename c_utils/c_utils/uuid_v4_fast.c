#include "uuid_v4_fast.h"
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

uuid_v4_fast_error_t uuid_v4_fast_create(uuid_v4_fast_ctx_t** ctx, const uuid_v4_fast_config_t* config) {
    if (!ctx) return UUID_V4_FAST_INVALID_PARAMS;
    
    *ctx = malloc(sizeof(uuid_v4_fast_ctx_t));
    if (!*ctx) return UUID_V4_FAST_MEMORY_ERROR;
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.use_hyphens = true;
        (*ctx)->config.uppercase = false;
        (*ctx)->config.use_entropy_pool = false;
        (*ctx)->config.prng_seed_size = 16;
    }
    
    // 初始化 PRNG 状态
    (*ctx)->prng_state[0] = (uint64_t)rand() << 32 | rand();
    (*ctx)->prng_state[1] = (uint64_t)rand() << 32 | rand();
    (*ctx)->last_error = UUID_V4_FAST_OK;
    (*ctx)->generate_count = 0;
    
    return UUID_V4_FAST_OK;
}

void uuid_v4_fast_destroy(uuid_v4_fast_ctx_t* ctx) {
    free(ctx);
}

uuid_v4_fast_error_t uuid_v4_fast_generate(uuid_v4_fast_ctx_t* ctx, uint8_t *uuid) {
    if (!ctx || !uuid) return UUID_V4_FAST_INVALID_PARAMS;
    
    // 使用 xorshift128+ 生成随机数
    uint64_t r1 = xorshift128plus(ctx->prng_state);
    uint64_t r2 = xorshift128plus(ctx->prng_state);
    
    // 填充 UUID
    for (int i = 0; i < 8; i++) {
        uuid[i] = (r1 >> (i * 8)) & 0xFF;
        uuid[i + 8] = (r2 >> (i * 8)) & 0xFF;
    }
    
    // 设置版本和变体
    uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant 1
    
    ctx->generate_count++;
    return UUID_V4_FAST_OK;
}

uuid_v4_fast_error_t uuid_v4_fast_generate_string(uuid_v4_fast_ctx_t* ctx, char* out) {
    if (!ctx || !out) return UUID_V4_FAST_INVALID_PARAMS;
    
    uint8_t uuid[16];
    uuid_v4_fast_error_t err = uuid_v4_fast_generate(ctx, uuid);
    if (err != UUID_V4_FAST_OK) return err;
    
    const char *fmt = ctx->config.uppercase ? 
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X" :
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x";
    
    if (ctx->config.use_hyphens) {
        sprintf(out, fmt,
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5], uuid[6], uuid[7],
            uuid[8], uuid[9], uuid[10], uuid[11],
            uuid[12], uuid[13], uuid[14], uuid[15]);
    } else {
        sprintf(out, fmt,
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5], uuid[6], uuid[7],
            uuid[8], uuid[9], uuid[10], uuid[11],
            uuid[12], uuid[13], uuid[14], uuid[15]);
    }
    
    return UUID_V4_FAST_OK;
}

uuid_v4_fast_error_t uuid_v4_fast_generate_batch(uuid_v4_fast_ctx_t* ctx, uint8_t* uuids, size_t count) {
    if (!ctx || !uuids || count == 0) return UUID_V4_FAST_INVALID_PARAMS;
    
    for (size_t i = 0; i < count; i++) {
        uuid_v4_fast_error_t err = uuid_v4_fast_generate(ctx, uuids + i * 16);
        if (err != UUID_V4_FAST_OK) return err;
    }
    
    return UUID_V4_FAST_OK;
}

uuid_v4_fast_error_t uuid_v4_fast_reset_seed(uuid_v4_fast_ctx_t* ctx, const uint8_t* seed, size_t seed_len) {
    if (!ctx) return UUID_V4_FAST_INVALID_PARAMS;
    
    // 使用种子重新初始化 PRNG
    uint64_t s1 = 0, s2 = 0;
    for (size_t i = 0; i < seed_len && i < 8; i++) {
        s1 = (s1 << 8) | seed[i];
    }
    for (size_t i = 8; i < seed_len && i < 16; i++) {
        s2 = (s2 << 8) | seed[i];
    }
    
    ctx->prng_state[0] = s1 ? s1 : 1;
    ctx->prng_state[1] = s2 ? s2 : 2;
    
    return UUID_V4_FAST_OK;
}

uint64_t uuid_v4_fast_get_generate_count(uuid_v4_fast_ctx_t* ctx) {
    return ctx ? ctx->generate_count : 0;
}

uuid_v4_fast_error_t uuid_v4_fast_get_last_error(uuid_v4_fast_ctx_t* ctx) {
    return ctx ? ctx->last_error : UUID_V4_FAST_INVALID_PARAMS;
}

const char* uuid_v4_fast_strerror(uuid_v4_fast_error_t error) {
    switch (error) {
        case UUID_V4_FAST_OK: return "Success";
        case UUID_V4_FAST_INVALID_PARAMS: return "Invalid parameters";
        case UUID_V4_FAST_MEMORY_ERROR: return "Memory error";
        case UUID_V4_FAST_GENERATION_ERROR: return "Generation error";
        default: return "Unknown error";
    }
}
