#include "aes_tiny.h"
#include <string.h>
#include <stdlib.h>

// 这是一个占位实现，实际 AES 逻辑较长
void aes_tiny_encrypt_simple(const uint8_t *in, const uint8_t *key, uint8_t *out) {
    // 简单异或作为加密示例 (生产环境需完整 AES 实现)
    for (int i = 0; i < 16; i++) out[i] = in[i] ^ key[i];
}

// 创建 AES tiny 上下文
aes_tiny_error_t aes_tiny_create(aes_tiny_ctx_t** ctx, const aes_tiny_config_t* config, const uint8_t* key, size_t key_len) {
    if (!ctx) {
        return AES_TINY_INVALID_PARAMS;
    }
    
    if (config && config->check_key_size) {
        if (key_len != 16 && key_len != 24 && key_len != 32) {
            return AES_TINY_INVALID_KEY_SIZE;
        }
    }
    
    *ctx = (aes_tiny_ctx_t*)malloc(sizeof(aes_tiny_ctx_t));
    if (!*ctx) {
        return AES_TINY_MEMORY_ERROR;
    }
    
    memset(*ctx, 0, sizeof(aes_tiny_ctx_t));
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.use_padding = true;
        (*ctx)->config.check_key_size = true;
        (*ctx)->config.check_block_size = true;
        (*ctx)->config.mode = AES_TINY_MODE_ECB;
        (*ctx)->config.key_size = AES_TINY_KEY_SIZE_128;
    }
    
    // 复制密钥
    if (key) {
        memcpy((*ctx)->round_keys, key, key_len > 240 ? 240 : key_len);
    }
    
    (*ctx)->rounds = (key_len == 16) ? 10 : (key_len == 24) ? 12 : 14;
    (*ctx)->last_error = AES_TINY_OK;
    
    return AES_TINY_OK;
}

// 销毁 AES tiny 上下文
void aes_tiny_destroy(aes_tiny_ctx_t* ctx) {
    if (ctx) {
        memset(ctx, 0, sizeof(aes_tiny_ctx_t));
        free(ctx);
    }
}

// AES 加密（单个块）
aes_tiny_error_t aes_tiny_encrypt_block(aes_tiny_ctx_t* ctx, const uint8_t *in, uint8_t *out) {
    if (!ctx || !in || !out) {
        return AES_TINY_INVALID_PARAMS;
    }
    
    // 简单异或加密（占位实现）
    for (int i = 0; i < 16; i++) {
        out[i] = in[i] ^ ctx->round_keys[i % 16];
    }
    
    ctx->encrypt_count++;
    return AES_TINY_OK;
}

// AES 解密（单个块）
aes_tiny_error_t aes_tiny_decrypt_block(aes_tiny_ctx_t* ctx, const uint8_t *in, uint8_t *out) {
    if (!ctx || !in || !out) {
        return AES_TINY_INVALID_PARAMS;
    }
    
    // 简单异或解密（与加密相同，占位实现）
    for (int i = 0; i < 16; i++) {
        out[i] = in[i] ^ ctx->round_keys[i % 16];
    }
    
    return AES_TINY_OK;
}

// AES 加密（多个块）
aes_tiny_error_t aes_tiny_encrypt(aes_tiny_ctx_t* ctx, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t* iv) {
    if (!ctx || !in || !out || !out_len) {
        return AES_TINY_INVALID_PARAMS;
    }
    
    size_t blocks = (in_len + 15) / 16;
    if (ctx->config.use_padding) {
        blocks++;
    }
    
    *out_len = blocks * 16;
    
    uint8_t block[16];
    uint8_t prev[16];
    
    if (iv) {
        memcpy(prev, iv, 16);
    } else {
        memset(prev, 0, 16);
    }
    
    for (size_t i = 0; i < in_len; i += 16) {
        memset(block, 0, 16);
        size_t copy_len = (in_len - i) > 16 ? 16 : (in_len - i);
        memcpy(block, in + i, copy_len);
        
        // CBC模式
        if (ctx->config.mode == AES_TINY_MODE_CBC) {
            for (int j = 0; j < 16; j++) {
                block[j] ^= prev[j];
            }
        }
        
        aes_tiny_encrypt_block(ctx, block, out + i);
        memcpy(prev, out + i, 16);
    }
    
    // 添加填充
    if (ctx->config.use_padding) {
        size_t pad_start = in_len;
        uint8_t pad_val = 16 - (in_len % 16);
        if (pad_val == 0) pad_val = 16;
        
        memset(block, pad_val, 16);
        memcpy(block, in + pad_start, in_len - pad_start);
        
        if (ctx->config.mode == AES_TINY_MODE_CBC) {
            for (int j = 0; j < 16; j++) {
                block[j] ^= prev[j];
            }
        }
        
        aes_tiny_encrypt_block(ctx, block, out + pad_start);
    }
    
    ctx->encrypt_count++;
    return AES_TINY_OK;
}

// AES 解密（多个块）
aes_tiny_error_t aes_tiny_decrypt(aes_tiny_ctx_t* ctx, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t* iv) {
    if (!ctx || !in || !out || !out_len) {
        return AES_TINY_INVALID_PARAMS;
    }
    
    if (in_len % 16 != 0) {
        return AES_TINY_INVALID_BLOCK_SIZE;
    }
    
    *out_len = in_len;
    
    uint8_t prev[16];
    if (iv) {
        memcpy(prev, iv, 16);
    } else {
        memset(prev, 0, 16);
    }
    
    for (size_t i = 0; i < in_len; i += 16) {
        uint8_t temp[16];
        memcpy(temp, in + i, 16);
        
        aes_tiny_decrypt_block(ctx, in + i, out + i);
        
        // CBC模式
        if (ctx->config.mode == AES_TINY_MODE_CBC) {
            for (int j = 0; j < 16; j++) {
                out[i + j] ^= prev[j];
            }
        }
        
        memcpy(prev, temp, 16);
    }
    
    // 移除填充
    if (ctx->config.use_padding && in_len > 0) {
        uint8_t pad_val = out[in_len - 1];
        if (pad_val > 0 && pad_val <= 16) {
            *out_len -= pad_val;
        }
    }
    
    ctx->decrypt_count++;
    return AES_TINY_OK;
}

// 批量加密
aes_tiny_error_t aes_tiny_encrypt_batch(aes_tiny_ctx_t* ctx, const uint8_t** in, const size_t* in_lens, size_t count, uint8_t** out, size_t* out_lens, const uint8_t* iv) {
    if (!ctx || !in || !in_lens || !out || !out_lens) {
        return AES_TINY_INVALID_PARAMS;
    }
    
    for (size_t i = 0; i < count; i++) {
        aes_tiny_error_t err = aes_tiny_encrypt(ctx, in[i], in_lens[i], out[i], &out_lens[i], iv);
        if (err != AES_TINY_OK) {
            return err;
        }
    }
    
    return AES_TINY_OK;
}

// 获取最后一次错误
aes_tiny_error_t aes_tiny_get_last_error(aes_tiny_ctx_t* ctx) {
    if (!ctx) {
        return AES_TINY_INVALID_PARAMS;
    }
    return ctx->last_error;
}

// 获取错误信息
const char* aes_tiny_strerror(aes_tiny_error_t error) {
    switch (error) {
        case AES_TINY_OK: return "Success";
        case AES_TINY_INVALID_PARAMS: return "Invalid parameters";
        case AES_TINY_INVALID_KEY_SIZE: return "Invalid key size";
        case AES_TINY_INVALID_BLOCK_SIZE: return "Invalid block size";
        case AES_TINY_MEMORY_ERROR: return "Memory error";
        case AES_TINY_ENCRYPT_ERROR: return "Encryption error";
        case AES_TINY_DECRYPT_ERROR: return "Decryption error";
        case AES_TINY_UNSUPPORTED_MODE: return "Unsupported mode";
        default: return "Unknown error";
    }
}
