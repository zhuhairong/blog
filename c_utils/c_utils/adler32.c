#include "adler32.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MOD_ADLER 65521

// 传统增量计算函数
uint32_t adler32_update(uint32_t adler, const void *data, size_t len) {
    uint32_t s1 = adler & 0xffff;
    uint32_t s2 = (adler >> 16) & 0xffff;
    const uint8_t *buf = (const uint8_t *)data;

    for (size_t n = 0; n < len; n++) {
        s1 = (s1 + buf[n]) % MOD_ADLER;
        s2 = (s2 + s1) % MOD_ADLER;
    }
    return (s2 << 16) | s1;
}

// 传统计算函数
uint32_t adler32_compute(const void *data, size_t len) {
    return adler32_update(1L, data, len);
}

// 创建 Adler-32 上下文
adler32_error_t adler32_create(adler32_ctx_t** ctx, const adler32_config_t* config) {
    if (!ctx) {
        return ADLER32_INVALID_PARAMS;
    }
    
    *ctx = (adler32_ctx_t*)malloc(sizeof(adler32_ctx_t));
    if (!*ctx) {
        return ADLER32_MEMORY_ERROR;
    }
    
    memset(*ctx, 0, sizeof(adler32_ctx_t));
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.check_overflow = true;
        (*ctx)->config.use_large_window = false;
        (*ctx)->config.verify_result = false;
        (*ctx)->config.max_batch_size = 1000;
        (*ctx)->config.max_file_size = 10 * 1024 * 1024;  // 10MB
    }
    
    (*ctx)->last_error = ADLER32_OK;
    
    return ADLER32_OK;
}

// 销毁 Adler-32 上下文
void adler32_destroy(adler32_ctx_t* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// 计算 Adler-32 校验和
adler32_error_t adler32_compute_safe(adler32_ctx_t* ctx, const void *data, size_t len, uint32_t* out) {
    if (!ctx || !data || !out) {
        return ADLER32_INVALID_PARAMS;
    }
    
    *out = adler32_compute(data, len);
    ctx->compute_count++;
    
    return ADLER32_OK;
}

// 增量计算 Adler-32
adler32_error_t adler32_update_safe(adler32_ctx_t* ctx, uint32_t adler, const void *data, size_t len, uint32_t* out) {
    if (!ctx || !data || !out) {
        return ADLER32_INVALID_PARAMS;
    }
    
    *out = adler32_update(adler, data, len);
    ctx->update_count++;
    
    return ADLER32_OK;
}

// 批量计算 Adler-32
adler32_error_t adler32_compute_batch(adler32_ctx_t* ctx, const void** data, const size_t* lengths, size_t count, uint32_t* out) {
    if (!ctx || !data || !lengths || !out) {
        return ADLER32_INVALID_PARAMS;
    }
    
    if (ctx->config.max_batch_size > 0 && count > ctx->config.max_batch_size) {
        return ADLER32_BUFFER_TOO_SMALL;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (data[i]) {
            out[i] = adler32_compute(data[i], lengths[i]);
        }
    }
    
    ctx->compute_count += count;
    
    return ADLER32_OK;
}

// 从文件计算 Adler-32
adler32_error_t adler32_compute_file(adler32_ctx_t* ctx, const char* filename, uint32_t* out) {
    if (!ctx || !filename || !out) {
        return ADLER32_INVALID_PARAMS;
    }
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        return ADLER32_FILE_ERROR;
    }
    
    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (ctx->config.max_file_size > 0 && (size_t)file_size > ctx->config.max_file_size) {
        fclose(fp);
        return ADLER32_BUFFER_TOO_SMALL;
    }
    
    uint8_t buffer[4096];
    uint32_t adler = ADLER32_INIT;
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        adler = adler32_update(adler, buffer, bytes_read);
    }
    
    fclose(fp);
    
    *out = adler;
    ctx->file_count++;
    ctx->compute_count++;
    
    return ADLER32_OK;
}

// 从内存块计算 Adler-32
adler32_error_t adler32_compute_block(adler32_ctx_t* ctx, const void* data, size_t len, size_t block_size, uint32_t* out) {
    if (!ctx || !data || !out) {
        return ADLER32_INVALID_PARAMS;
    }
    
    if (block_size == 0) {
        block_size = 4096;
    }
    
    uint32_t adler = ADLER32_INIT;
    const uint8_t* ptr = (const uint8_t*)data;
    
    for (size_t offset = 0; offset < len; offset += block_size) {
        size_t chunk_size = (offset + block_size > len) ? (len - offset) : block_size;
        adler = adler32_update(adler, ptr + offset, chunk_size);
    }
    
    *out = adler;
    ctx->compute_count++;
    
    return ADLER32_OK;
}

// 重置 Adler-32 上下文
void adler32_reset(adler32_ctx_t* ctx) {
    if (ctx) {
        ctx->last_error = ADLER32_OK;
    }
}

// 获取最后一次错误
adler32_error_t adler32_get_last_error(adler32_ctx_t* ctx) {
    if (!ctx) {
        return ADLER32_INVALID_PARAMS;
    }
    return ctx->last_error;
}

// 获取错误信息
const char* adler32_strerror(adler32_error_t error) {
    switch (error) {
        case ADLER32_OK: return "Success";
        case ADLER32_INVALID_PARAMS: return "Invalid parameters";
        case ADLER32_MEMORY_ERROR: return "Memory error";
        case ADLER32_FILE_ERROR: return "File error";
        case ADLER32_BUFFER_TOO_SMALL: return "Buffer too small";
        case ADLER32_OVERFLOW: return "Overflow";
        default: return "Unknown error";
    }
}
