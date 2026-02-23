#include "crc32.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// CRC32 标准表
static uint32_t crc32_standard_table[256];
static uint32_t crc32c_table[256];
static int tables_computed = 0;

// 生成 CRC32 表
static void make_crc32_table(uint32_t *table, uint32_t polynomial) {
    uint32_t c;
    for (int n = 0; n < 256; n++) {
        c = (uint32_t)n;
        for (int k = 0; k < 8; k++) {
            if (c & 1) c = polynomial ^ (c >> 1);
            else c = c >> 1;
        }
        table[n] = c;
    }
}

// 初始化所有表
static void init_tables(void) {
    if (tables_computed) return;
    make_crc32_table(crc32_standard_table, 0xedb88320L);
    make_crc32_table(crc32c_table, 0x82f63b78L);
    tables_computed = 1;
}

// 初始化 CRC32 上下文
bool crc32_init(crc32_context_t *ctx, crc32_variant_t variant, crc32_error_t *error) {
    if (!ctx) {
        if (error) *error = CRC32_ERROR_INVALID_PARAM;
        return false;
    }
    
    init_tables();
    
    switch (variant) {
        case CRC32_STANDARD:
            ctx->table = crc32_standard_table;
            break;
        case CRC32_C:
            ctx->table = crc32c_table;
            break;
        default:
            if (error) *error = CRC32_ERROR_UNSUPPORTED_VARIANT;
            return false;
    }
    
    ctx->variant = variant;
    ctx->crc = 0xffffffffL;
    
    if (error) *error = CRC32_OK;
    return true;
}

// 更新 CRC32 计算
bool crc32_update(crc32_context_t *ctx, const void *data, size_t len) {
    if (!ctx || !data) {
        return false;
    }
    
    const uint8_t *p = (const uint8_t *)data;
    while (len--) {
        ctx->crc = ctx->table[(ctx->crc ^ *p++) & 0xff] ^ (ctx->crc >> 8);
    }
    
    return true;
}

// 获取最终 CRC32 值
uint32_t crc32_final(crc32_context_t *ctx) {
    if (!ctx) return 0;
    return ctx->crc ^ 0xffffffffL;
}

// 计算数据的 CRC32 值
uint32_t crc32_compute(const void *data, size_t len, crc32_variant_t variant, crc32_error_t *error) {
    if (!data) {
        if (error) *error = CRC32_ERROR_INVALID_PARAM;
        return 0;
    }
    
    crc32_context_t ctx;
    if (!crc32_init(&ctx, variant, error)) {
        return 0;
    }
    
    crc32_update(&ctx, data, len);
    
    if (error) *error = CRC32_OK;
    return crc32_final(&ctx);
}

// 计算文件的 CRC32 值
uint32_t crc32_compute_file(const char *filename, crc32_variant_t variant, crc32_error_t *error) {
    if (!filename) {
        if (error) *error = CRC32_ERROR_INVALID_PARAM;
        return 0;
    }
    
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        if (error) *error = CRC32_ERROR_FILE_OPEN;
        return 0;
    }
    
    crc32_context_t ctx;
    if (!crc32_init(&ctx, variant, error)) {
        fclose(fp);
        return 0;
    }
    
    uint8_t buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        crc32_update(&ctx, buffer, n);
    }
    
    if (ferror(fp)) {
        fclose(fp);
        if (error) *error = CRC32_ERROR_FILE_READ;
        return 0;
    }
    
    fclose(fp);
    
    if (error) *error = CRC32_OK;
    return crc32_final(&ctx);
}

// 获取 CRC32 表
const uint32_t* crc32_get_table(crc32_variant_t variant) {
    init_tables();
    switch (variant) {
        case CRC32_STANDARD:
            return crc32_standard_table;
        case CRC32_C:
            return crc32c_table;
        default:
            return NULL;
    }
}

// 获取错误信息
const char* crc32_strerror(crc32_error_t error) {
    switch (error) {
        case CRC32_OK:
            return "Success";
        case CRC32_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case CRC32_ERROR_FILE_OPEN:
            return "Failed to open file";
        case CRC32_ERROR_FILE_READ:
            return "Failed to read file";
        case CRC32_ERROR_UNSUPPORTED_VARIANT:
            return "Unsupported CRC32 variant";
        default:
            return "Unknown error";
    }
}
