#ifndef C_UTILS_LZW_H
#define C_UTILS_LZW_H

#include <stddef.h>
#include <stdbool.h>

// LZW 错误码
typedef enum {
    LZW_OK = 0,
    LZW_INVALID_INPUT = -1,
    LZW_MEMORY_ERROR = -2,
    LZW_ENCODE_ERROR = -3,
    LZW_DECODE_ERROR = -4,
    LZW_BUFFER_TOO_SMALL = -5,
    LZW_TABLE_FULL = -6,
    LZW_INVALID_CODE = -7,
    LZW_BIT_WIDTH_ERROR = -8
} lzw_error_t;

// LZW 配置
typedef struct {
    size_t max_code_size;
    size_t initial_code_size;
    size_t max_dictionary_size;
    bool enable_reset;
    bool use_variable_length;
    bool enable_stats;
} lzw_config_t;

// LZW 统计信息
typedef struct {
    size_t input_size;
    size_t output_size;
    size_t dictionary_size;
    size_t reset_count;
    double compression_ratio;
} lzw_stats_t;

// 极简 LZW 压缩 (仅示意)
size_t lzw_encode(const unsigned char *in, size_t in_len, unsigned char *out);

// 带错误处理的压缩
lzw_error_t lzw_encode_ex(const unsigned char *input, size_t input_len, unsigned char *output, 
                         size_t output_size, size_t *output_len, const lzw_config_t *config);

// LZW 解压缩
lzw_error_t lzw_decode(const unsigned char *input, size_t input_len, unsigned char *output, 
                      size_t output_size, size_t *output_len);

// 带错误处理的解压缩
lzw_error_t lzw_decode_ex(const unsigned char *input, size_t input_len, unsigned char *output, 
                         size_t output_size, size_t *output_len, const lzw_config_t *config);

// 获取默认配置
void lzw_get_default_config(lzw_config_t *config);

// 计算压缩率
double lzw_calculate_ratio(size_t input_size, size_t output_size);

#endif // C_UTILS_LZW_H
