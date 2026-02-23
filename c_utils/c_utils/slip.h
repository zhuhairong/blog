#ifndef C_UTILS_SLIP_H
#define C_UTILS_SLIP_H

#include <stddef.h>
#include <stdbool.h>

// SLIP 特殊字符
#define SLIP_END             0xC0
#define SLIP_ESC             0xDB
#define SLIP_ESC_END         0xDC
#define SLIP_ESC_ESC         0xDD

typedef enum {
    SLIP_OK = 0,
    SLIP_ERROR_INVALID_PARAMS = -1,
    SLIP_ERROR_ENCODE = -2,
    SLIP_ERROR_DECODE = -3,
    SLIP_ERROR_BUFFER_TOO_SMALL = -4,
    SLIP_ERROR_INVALID_CHAR = -5,
    SLIP_ERROR_FILE_OPEN = -6,
    SLIP_ERROR_FILE_READ = -7,
    SLIP_ERROR_FILE_WRITE = -8,
    SLIP_ERROR_MEMORY = -9,
    SLIP_ERROR_INCOMPLETE_PACKET = -10
} slip_error_t;

typedef struct {
    bool add_end_delimiter;     // 添加结束分隔符
    bool add_start_delimiter;   // 添加开始分隔符
    bool enable_crc;            // 启用CRC校验
    bool enable_escape;         // 启用转义
    size_t max_packet_size;     // 最大数据包大小
    size_t buffer_size;         // 缓冲区大小
} slip_config_t;

typedef struct {
    slip_error_t last_error;     // 最后一次错误
    size_t total_encoded;        // 总编码字节数
    size_t total_decoded;        // 总解码字节数
    size_t packets_encoded;      // 编码包数
    size_t packets_decoded;      // 解码包数
    bool is_initialized;         // 是否初始化
    size_t current_packet_size;  // 当前数据包大小
} slip_state_t;

/**
 * @brief SLIP (Serial Line IP) 编码
 * @param in 输入数据
 * @param len 输入长度
 * @param out 输出缓冲区
 * @return 编码后的长度
 */
size_t slip_encode(const unsigned char *in, size_t len, unsigned char *out);

/**
 * @brief 增强版SLIP编码
 * @param in 输入数据
 * @param len 输入长度
 * @param out 输出缓冲区
 * @param out_len 输出缓冲区长度
 * @param config 配置选项
 * @param state 状态输出
 * @return 编码后的长度，失败返回0
 */
size_t slip_encode_ex(const unsigned char *in, size_t len, unsigned char *out, size_t out_len, const slip_config_t *config, slip_state_t *state);

/**
 * @brief SLIP解码
 * @param in 输入数据
 * @param len 输入长度
 * @param out 输出缓冲区
 * @param out_len 输出缓冲区长度
 * @param state 状态输出
 * @return 解码后的长度，失败返回0
 */
size_t slip_decode(const unsigned char *in, size_t len, unsigned char *out, size_t out_len, slip_state_t *state);

/**
 * @brief 增强版SLIP解码
 * @param in 输入数据
 * @param len 输入长度
 * @param out 输出缓冲区
 * @param out_len 输出缓冲区长度
 * @param config 配置选项
 * @param state 状态输出
 * @return 解码后的长度，失败返回0
 */
size_t slip_decode_ex(const unsigned char *in, size_t len, unsigned char *out, size_t out_len, const slip_config_t *config, slip_state_t *state);

/**
 * @brief 编码文件
 * @param input_file 输入文件名
 * @param output_file 输出文件名
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
slip_error_t slip_encode_file(const char *input_file, const char *output_file, const slip_config_t *config, slip_state_t *state);

/**
 * @brief 解码文件
 * @param input_file 输入文件名
 * @param output_file 输出文件名
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
slip_error_t slip_decode_file(const char *input_file, const char *output_file, const slip_config_t *config, slip_state_t *state);

/**
 * @brief 计算编码所需的最大缓冲区大小
 * @param input_len 输入长度
 * @param config 配置选项
 * @return 最大所需缓冲区大小
 */
size_t slip_calculate_max_encoded_size(size_t input_len, const slip_config_t *config);

/**
 * @brief 重置SLIP状态
 * @param state 状态结构
 */
void slip_reset_state(slip_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* slip_strerror(const slip_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void slip_config_init(slip_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void slip_state_init(slip_state_t *state);

#endif // C_UTILS_SLIP_H
