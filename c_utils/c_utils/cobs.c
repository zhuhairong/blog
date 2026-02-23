#include "cobs.h"
#include <string.h>

// COBS 编码
// 参考: https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
cobs_error_t cobs_encode(const unsigned char *in, size_t len, unsigned char *out, size_t out_buf_size, size_t *written) {
    if (!in || !out || !written) {
        return COBS_ERROR_INVALID_INPUT;
    }
    
    // 计算所需的最大输出缓冲区大小
    size_t max_out = cobs_max_encoded_size(len);
    if (out_buf_size < max_out) {
        return COBS_ERROR_BUFFER_TOO_SMALL;
    }
    
    size_t read_index = 0;
    size_t write_index = 1;
    size_t code_index = 0;
    unsigned char code = 1;
    
    while (read_index < len) {
        if (in[read_index] == 0) {
            out[code_index] = code;
            code = 1;
            code_index = write_index++;
            read_index++;
        } else {
            out[write_index++] = in[read_index++];
            code++;
            if (code == 0xFF) {
                out[code_index] = code;
                code = 1;
                code_index = write_index++;
            }
        }
    }
    
    out[code_index] = code;
    *written = write_index;
    
    return COBS_OK;
}

// COBS 解码
cobs_error_t cobs_decode(const unsigned char *in, size_t len, unsigned char *out, size_t out_buf_size, size_t *written) {
    if (!in || !out || !written) {
        return COBS_ERROR_INVALID_INPUT;
    }
    
    if (len == 0) {
        *written = 0;
        return COBS_OK;
    }
    
    size_t read_index = 0;
    size_t write_index = 0;
    
    while (read_index < len) {
        unsigned char code = in[read_index++];
        
        if (code == 0) {
            return COBS_ERROR_INVALID_ENCODED_DATA;
        }
        
        // 检查缓冲区是否足够
        if (write_index + code - 1 > out_buf_size) {
            return COBS_ERROR_BUFFER_TOO_SMALL;
        }
        
        for (int i = 1; i < code; i++) {
            if (read_index >= len) {
                return COBS_ERROR_UNTERMINATED_PACKET;
            }
            out[write_index++] = in[read_index++];
        }
        
        // 如果不是块的最后一个字节，添加一个零字节
        if (code < 0xFF && read_index < len) {
            if (write_index >= out_buf_size) {
                return COBS_ERROR_BUFFER_TOO_SMALL;
            }
            out[write_index++] = 0;
        }
    }
    
    *written = write_index;
    return COBS_OK;
}

// 计算 COBS 编码所需的最大缓冲区大小
size_t cobs_max_encoded_size(size_t len) {
    // COBS 编码最坏情况下会增加 len/254 + 1 字节的开销
    return len + (len / 254) + 1;
}

// 计算 COBS 解码所需的最大缓冲区大小
size_t cobs_max_decoded_size(size_t len) {
    // 解码后的数据长度不会超过编码长度
    return len;
}

// 检查 COBS 编码数据的有效性
bool cobs_validate(const unsigned char *in, size_t len) {
    if (!in || len == 0) return false;
    
    size_t read_index = 0;
    
    while (read_index < len) {
        unsigned char code = in[read_index++];
        
        if (code == 0) {
            return false;
        }
        
        // 跳过 code-1 个数据字节
        read_index += code - 1;
        
        if (read_index > len) {
            return false;
        }
    }
    
    return true;
}

// 获取错误信息
const char* cobs_error_string(cobs_error_t error) {
    switch (error) {
        case COBS_OK:
            return "Success";
        case COBS_ERROR_INVALID_INPUT:
            return "Invalid input parameters";
        case COBS_ERROR_INVALID_ENCODED_DATA:
            return "Invalid encoded data";
        case COBS_ERROR_BUFFER_TOO_SMALL:
            return "Buffer too small";
        case COBS_ERROR_UNTERMINATED_PACKET:
            return "Unterminated packet";
        default:
            return "Unknown error";
    }
}
