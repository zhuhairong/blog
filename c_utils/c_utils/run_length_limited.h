#ifndef C_UTILS_RUN_LENGTH_LIMITED_H
#define C_UTILS_RUN_LENGTH_LIMITED_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief RLL 错误码
 */
typedef enum {
    RLL_OK = 0,                  /**< 成功 */
    RLL_ERROR_NULL_PTR,          /**< 空指针错误 */
    RLL_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RLL_ERROR_INPUT_TOO_LARGE,   /**< 输入过大 */
    RLL_ERROR_OUTPUT_TOO_SMALL,  /**< 输出过小 */
    RLL_ERROR_INVALID_RUN_LENGTH, /**< 无效的运行长度 */
    RLL_ERROR_ENCODING_FAILED,   /**< 编码失败 */
    RLL_ERROR_DECODING_FAILED,   /**< 解码失败 */
    RLL_ERROR_MAX                /**< 最大错误码 */
} rll_error_t;

/**
 * @brief RLL 配置
 */
typedef struct {
    size_t min_run_length;        /**< 最小运行长度 */
    size_t max_run_length;        /**< 最大运行长度 */
    bool use_optimized;           /**< 是否使用优化模式 */
    bool check_input;             /**< 是否检查输入数据 */
    bool check_output;            /**< 是否检查输出空间 */
    size_t max_input_size;        /**< 最大输入大小 */
    size_t max_output_size;       /**< 最大输出大小 */
    bool use_variable_length;     /**< 是否使用可变长度编码 */
} rll_config_t;

/**
 * @brief RLL 编码结果
 */
typedef struct {
    size_t input_size;            /**< 输入大小 */
    size_t output_size;           /**< 输出大小 */
    size_t run_count;             /**< 运行次数 */
    double compression_ratio;     /**< 压缩比 */
    bool compressed;              /**< 是否成功压缩 */
    bool optimized;               /**< 是否使用优化 */
} rll_result_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
rll_config_t rll_default_config(void);

/**
 * @brief 获取 RLL(1,7) 默认配置
 * @return RLL(1,7) 默认配置
 */
rll_config_t rll_1_7_default_config(void);

/**
 * @brief RLL 编码基础示例
 * @param in 输入数据
 * @param len 输入长度
 * @param out 输出缓冲区
 * @return 编码后长度，失败返回 0
 */
size_t rll_encode_basic(const unsigned char *in, size_t len, unsigned char *out);

/**
 * @brief RLL 编码
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @param out_size 输出缓冲区大小
 * @param config 配置选项
 * @param result 编码结果输出
 * @param error 错误码输出
 * @return 编码后长度，失败返回 0
 */
size_t rll_encode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, const rll_config_t *config, rll_result_t *result, rll_error_t *error);

/**
 * @brief RLL 解码
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @param out_size 输出缓冲区大小
 * @param config 配置选项
 * @param result 解码结果输出
 * @param error 错误码输出
 * @return 解码后长度，失败返回 0
 */
size_t rll_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, const rll_config_t *config, rll_result_t *result, rll_error_t *error);

/**
 * @brief RLL(1,7) 编码
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @param out_size 输出缓冲区大小
 * @param result 编码结果输出
 * @param error 错误码输出
 * @return 编码后长度，失败返回 0
 */
size_t rll_1_7_encode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, rll_result_t *result, rll_error_t *error);

/**
 * @brief RLL(1,7) 解码
 * @param in 输入数据
 * @param in_len 输入长度
 * @param out 输出缓冲区
 * @param out_size 输出缓冲区大小
 * @param result 解码结果输出
 * @param error 错误码输出
 * @return 解码后长度，失败返回 0
 */
size_t rll_1_7_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t out_size, rll_result_t *result, rll_error_t *error);

/**
 * @brief 计算 RLL 编码后的最大可能大小
 * @param in_len 输入长度
 * @param config 配置选项
 * @return 最大可能的输出大小
 */
size_t rll_calculate_max_output_size(size_t in_len, const rll_config_t *config);

/**
 * @brief 验证编码数据
 * @param in 编码数据
 * @param in_len 编码数据长度
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否有效
 */
bool rll_validate_encoded_data(const unsigned char *in, size_t in_len, const rll_config_t *config, rll_error_t *error);

/**
 * @brief 压缩文件
 * @param input_filename 输入文件名
 * @param output_filename 输出文件名
 * @param config 配置选项
 * @param result 压缩结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool rll_compress_file(const char *input_filename, const char *output_filename, const rll_config_t *config, rll_result_t *result, rll_error_t *error);

/**
 * @brief 解压缩文件
 * @param input_filename 输入文件名
 * @param output_filename 输出文件名
 * @param config 配置选项
 * @param result 解压缩结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool rll_decompress_file(const char *input_filename, const char *output_filename, const rll_config_t *config, rll_result_t *result, rll_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* rll_error_string(rll_error_t error);

#endif // C_UTILS_RUN_LENGTH_LIMITED_H
