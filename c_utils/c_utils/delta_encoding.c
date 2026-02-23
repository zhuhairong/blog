#include "delta_encoding.h"
#include <string.h>
#include <stdint.h>

// 默认增量编码配置
delta_encoding_config_t delta_encoding_default_config(delta_type_t type) {
    delta_encoding_config_t config = {
        .type = type,
        .use_variable_length = false,
        .allow_negative_deltas = true,
        .block_size = 1024
    };
    return config;
}

// 增量编码（整数类型）
bool delta_encode(const void *in, void *out, size_t n, delta_type_t type, delta_encoding_error_t *error) {
    if (!in || !out || n == 0) {
        if (error) *error = DELTA_ENCODING_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 简单实现：仅支持int32
    if (type == DELTA_TYPE_INT32) {
        const int32_t *input = (const int32_t *)in;
        int32_t *output = (int32_t *)out;
        
        if (n > 0) {
            output[0] = input[0];
            for (size_t i = 1; i < n; i++) {
                output[i] = input[i] - input[i-1];
            }
        }
    } else {
        if (error) *error = DELTA_ENCODING_ERROR_UNSUPPORTED_TYPE;
        return false;
    }
    
    if (error) *error = DELTA_ENCODING_OK;
    return true;
}

// 增量解码（整数类型）
bool delta_decode(const void *in, void *out, size_t n, delta_type_t type, delta_encoding_error_t *error) {
    if (!in || !out || n == 0) {
        if (error) *error = DELTA_ENCODING_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 简单实现：仅支持int32
    if (type == DELTA_TYPE_INT32) {
        const int32_t *input = (const int32_t *)in;
        int32_t *output = (int32_t *)out;
        
        if (n > 0) {
            output[0] = input[0];
            for (size_t i = 1; i < n; i++) {
                output[i] = output[i-1] + input[i];
            }
        }
    } else {
        if (error) *error = DELTA_ENCODING_ERROR_UNSUPPORTED_TYPE;
        return false;
    }
    
    if (error) *error = DELTA_ENCODING_OK;
    return true;
}

// 增量编码（带配置）
bool delta_encode_ex(const void *in, void *out, size_t n, const delta_encoding_config_t *config, size_t *encoded_size, delta_encoding_error_t *error) {
    if (!in || !out || !config || n == 0) {
        if (error) *error = DELTA_ENCODING_ERROR_INVALID_PARAM;
        return false;
    }
    
    bool result = delta_encode(in, out, n, config->type, error);
    if (result && encoded_size) {
        *encoded_size = n * sizeof(int32_t); // 简化处理
    }
    return result;
}

// 增量解码（带配置）
bool delta_decode_ex(const void *in, void *out, size_t n, const delta_encoding_config_t *config, size_t *decoded_size, delta_encoding_error_t *error) {
    if (!in || !out || !config || n == 0) {
        if (error) *error = DELTA_ENCODING_ERROR_INVALID_PARAM;
        return false;
    }
    
    bool result = delta_decode(in, out, n, config->type, error);
    if (result && decoded_size) {
        *decoded_size = n * sizeof(int32_t); // 简化处理
    }
    return result;
}

// 获取错误信息
const char* delta_encoding_strerror(delta_encoding_error_t error) {
    switch (error) {
        case DELTA_ENCODING_OK: return "Success";
        case DELTA_ENCODING_ERROR_INVALID_PARAM: return "Invalid parameter";
        case DELTA_ENCODING_ERROR_INSUFFICIENT_DATA: return "Insufficient data";
        case DELTA_ENCODING_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case DELTA_ENCODING_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case DELTA_ENCODING_ERROR_UNSUPPORTED_TYPE: return "Unsupported data type";
        default: return "Unknown error";
    }
}
