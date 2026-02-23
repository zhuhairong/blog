#ifndef C_UTILS_RLE_H
#define C_UTILS_RLE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief RLE 错误码
 */
typedef enum {
    RLE_OK = 0,                  /**< 成功 */
    RLE_ERROR_NULL_PTR,          /**< 空指针错误 */
    RLE_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RLE_ERROR_INPUT_TOO_LARGE,   /**< 输入过大 */
    RLE_ERROR_OUTPUT_TOO_SMALL,  /**< 输出过小 */
    RLE_ERROR_INVALID_ENCODED_DATA, /**< 无效的编码数据 */
    RLE_ERROR_COMPRESSION_FAILED, /**< 压缩失败 */
    RLE_ERROR_DECOMPRESSION_FAILED, /**< 解压缩失败 */
    RLE_ERROR_MAX                /**< 最大错误码 */
} rle_error_t;

/**
 * @brief RLE 配置
 */
typedef struct {
    bool use_optimized;           /**< 是否使用优化模式 */
    bool use_variable_length;     /**< 是否使用可变长度计数 */
    bool check_input;             /**< 是否检查输入数据 */
    bool check_output;            /**< 是否检查输出空间 */
    size_t max_run_length;        /**< 最大运行长度 */
    size_t min_run_length;        /**< 最小运行长度 */
    size_t max_input_size;        /**< 最大输入大小 */
    size_t max_output_size;       /**< 最大输出大小 */
} rle_config_t;

/**
 * @brief RLE 压缩结果
 */
typedef struct {
    size_t input_size;            /**< 输入大小 */
    size_t output_size;           /**< 输出大小 */
    size_t run_count;             /**< 运行次数 */
    double compression_ratio;     /**< 压缩比 */
    bool compressed;              /**< 是否成功压缩 */
    bool optimized;               /**< 是否使用优化 */
} rle_result_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
rle_config_t rle_default_config(void);

/**
 * @brief RLE 压缩
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @return 压缩后长度，失败返回 0
 */
size_t rle_encode(const unsigned char *in, size_t in_len, unsigned char *out);

/**
 * @brief RLE 压缩（带错误处理）
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @param out_size 输出缓冲区大小
 * @param config 配置选项
 * @param result 压缩结果输出
 * @param error 错误码输出
 * @return 压缩后长度，失败返回 0
 */
size_t rle_encode_ex(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, const rle_config_t *config, rle_result_t *result, rle_error_t *error);

/**
 * @brief RLE 解压缩
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @return 解压缩后长度，失败返回 0
 */
size_t rle_decode(const unsigned char *in, size_t in_len, unsigned char *out);

/**
 * @brief RLE 解压缩（带错误处理）
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @param out_size 输出缓冲区大小
 * @param config 配置选项
 * @param result 解压缩结果输出
 * @param error 错误码输出
 * @return 解压缩后长度，失败返回 0
 */
size_t rle_decode_ex(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, const rle_config_t *config, rle_result_t *result, rle_error_t *error);

/**
 * @brief 计算 RLE 压缩后的最大可能大小
 * @param in_len 输入长度
 * @param config 配置选项
 * @return 最大可能的输出大小
 */
size_t rle_calculate_max_output_size(size_t in_len, const rle_config_t *config);

/**
 * @brief 预测 RLE 压缩率
 * @param in 输入数据
 * @param in_len 输入长度
 * @param config 配置选项
 * @param estimated_ratio 估计压缩比输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool rle_predict_compression_ratio(const unsigned char *in, size_t in_len, const rle_config_t *config, double *estimated_ratio, rle_error_t *error);

/**
 * @brief 验证编码数据
 * @param in 编码数据
 * @param in_len 编码数据长度
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否有效
 */
bool rle_validate_encoded_data(const unsigned char *in, size_t in_len, const rle_config_t *config, rle_error_t *error);

/**
 * @brief 压缩文件
 * @param input_filename 输入文件名
 * @param output_filename 输出文件名
 * @param config 配置选项
 * @param result 压缩结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool rle_compress_file(const char *input_filename, const char *output_filename, const rle_config_t *config, rle_result_t *result, rle_error_t *error);

/**
 * @brief 解压缩文件
 * @param input_filename 输入文件名
 * @param output_filename 输出文件名
 * @param config 配置选项
 * @param result 解压缩结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool rle_decompress_file(const char *input_filename, const char *output_filename, const rle_config_t *config, rle_result_t *result, rle_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* rle_error_string(rle_error_t error);

#endif // C_UTILS_RLE_H
