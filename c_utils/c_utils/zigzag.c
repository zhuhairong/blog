#include "zigzag.h"
#include <stdlib.h>
#include <string.h>

// 传统编码函数
uint64_t zigzag_encode(int64_t n) {
    return (uint64_t)((n << 1) ^ (n >> 63));
}

// 传统解码函数
int64_t zigzag_decode(uint64_t n) {
    return (int64_t)((n >> 1) ^ -(int64_t)(n & 1));
}

// 创建 ZigZag 上下文
zigzag_error_t zigzag_create(zigzag_ctx_t** ctx, const zigzag_config_t* config) {
    if (!ctx) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    *ctx = (zigzag_ctx_t*)malloc(sizeof(zigzag_ctx_t));
    if (!*ctx) {
        return ZIGZAG_MEMORY_ERROR;
    }
    
    memset(*ctx, 0, sizeof(zigzag_ctx_t));
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.check_overflow = true;
        (*ctx)->config.use_signed = true;
        (*ctx)->config.max_batch_size = 1000;
    }
    
    (*ctx)->last_error = ZIGZAG_OK;
    
    return ZIGZAG_OK;
}

// 销毁 ZigZag 上下文
void zigzag_destroy(zigzag_ctx_t* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// 编码 int64_t 为 zigzag
zigzag_error_t zigzag_encode_int64(zigzag_ctx_t* ctx, int64_t n, uint64_t* out) {
    if (!ctx || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    *out = zigzag_encode(n);
    ctx->encode_count++;
    
    return ZIGZAG_OK;
}

// 解码 zigzag 为 int64_t
zigzag_error_t zigzag_decode_int64(zigzag_ctx_t* ctx, uint64_t n, int64_t* out) {
    if (!ctx || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    *out = zigzag_decode(n);
    ctx->decode_count++;
    
    return ZIGZAG_OK;
}

// 编码 int32_t 为 zigzag
zigzag_error_t zigzag_encode_int32(zigzag_ctx_t* ctx, int32_t n, uint32_t* out) {
    if (!ctx || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    *out = (uint32_t)zigzag_encode((int64_t)n);
    ctx->encode_count++;
    
    return ZIGZAG_OK;
}

// 解码 zigzag 为 int32_t
zigzag_error_t zigzag_decode_int32(zigzag_ctx_t* ctx, uint32_t n, int32_t* out) {
    if (!ctx || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    *out = (int32_t)zigzag_decode((uint64_t)n);
    ctx->decode_count++;
    
    return ZIGZAG_OK;
}

// 批量编码 int64_t 数组
zigzag_error_t zigzag_encode_batch_int64(zigzag_ctx_t* ctx, const int64_t* in, uint64_t* out, size_t count) {
    if (!ctx || !in || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    if (ctx->config.max_batch_size > 0 && count > ctx->config.max_batch_size) {
        return ZIGZAG_BUFFER_TOO_SMALL;
    }
    
    for (size_t i = 0; i < count; i++) {
        out[i] = zigzag_encode(in[i]);
        ctx->encode_count++;
    }
    
    return ZIGZAG_OK;
}

// 批量解码为 int64_t 数组
zigzag_error_t zigzag_decode_batch_int64(zigzag_ctx_t* ctx, const uint64_t* in, int64_t* out, size_t count) {
    if (!ctx || !in || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    if (ctx->config.max_batch_size > 0 && count > ctx->config.max_batch_size) {
        return ZIGZAG_BUFFER_TOO_SMALL;
    }
    
    for (size_t i = 0; i < count; i++) {
        out[i] = zigzag_decode(in[i]);
        ctx->decode_count++;
    }
    
    return ZIGZAG_OK;
}

// 批量编码 int32_t 数组
zigzag_error_t zigzag_encode_batch_int32(zigzag_ctx_t* ctx, const int32_t* in, uint32_t* out, size_t count) {
    if (!ctx || !in || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    if (ctx->config.max_batch_size > 0 && count > ctx->config.max_batch_size) {
        return ZIGZAG_BUFFER_TOO_SMALL;
    }
    
    for (size_t i = 0; i < count; i++) {
        out[i] = (uint32_t)zigzag_encode((int64_t)in[i]);
        ctx->encode_count++;
    }
    
    return ZIGZAG_OK;
}

// 批量解码为 int32_t 数组
zigzag_error_t zigzag_decode_batch_int32(zigzag_ctx_t* ctx, const uint32_t* in, int32_t* out, size_t count) {
    if (!ctx || !in || !out) {
        return ZIGZAG_INVALID_PARAMS;
    }
    
    if (ctx->config.max_batch_size > 0 && count > ctx->config.max_batch_size) {
        return ZIGZAG_BUFFER_TOO_SMALL;
    }
    
    for (size_t i = 0; i < count; i++) {
        out[i] = (int32_t)zigzag_decode((uint64_t)in[i]);
        ctx->decode_count++;
    }
    
    return ZIGZAG_OK;
}

// 获取最后一次错误
zigzag_error_t zigzag_get_last_error(zigzag_ctx_t* ctx) {
    if (!ctx) {
        return ZIGZAG_INVALID_PARAMS;
    }
    return ctx->last_error;
}

// 获取错误信息
const char* zigzag_strerror(zigzag_error_t error) {
    switch (error) {
        case ZIGZAG_OK: return "Success";
        case ZIGZAG_INVALID_PARAMS: return "Invalid parameters";
        case ZIGZAG_MEMORY_ERROR: return "Memory error";
        case ZIGZAG_BUFFER_TOO_SMALL: return "Buffer too small";
        case ZIGZAG_OVERFLOW: return "Overflow";
        case ZIGZAG_UNDERFLOW: return "Underflow";
        default: return "Unknown error";
    }
}
