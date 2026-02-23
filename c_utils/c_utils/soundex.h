#ifndef C_UTILS_SOUNDEX_H
#define C_UTILS_SOUNDEX_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// Soundex 常量
#define SOUNDEX_LENGTH 4
#define SOUNDEX_MAX_BUFFER_SIZE 64

typedef enum {
    SOUNDEX_OK = 0,
    SOUNDEX_ERROR_INVALID_PARAMS = -1,
    SOUNDEX_ERROR_ENCODE = -2,
    SOUNDEX_ERROR_DECODE = -3,
    SOUNDEX_ERROR_BUFFER_TOO_SMALL = -4,
    SOUNDEX_ERROR_STRING_EMPTY = -5,
    SOUNDEX_ERROR_MEMORY = -6,
    SOUNDEX_ERROR_FILE_OPEN = -7,
    SOUNDEX_ERROR_FILE_READ = -8,
    SOUNDEX_ERROR_FILE_WRITE = -9,
    SOUNDEX_ERROR_INVALID_CHAR = -10
} soundex_error_t;

typedef enum {
    SOUNDEX_ALGORITHM_STANDARD = 0,  // 标准 Soundex
    SOUNDEX_ALGORITHM_MODIFIED = 1,   // 改进版 Soundex
    SOUNDEX_ALGORITHM_METAPHONE = 2,  // Metaphone
    SOUNDEX_ALGORITHM_DOUBLE_METAPHONE = 3  // Double Metaphone
} soundex_algorithm_t;

typedef struct {
    soundex_algorithm_t algorithm;    // 算法类型
    bool ignore_case;               // 忽略大小写
    bool ignore_punctuation;        // 忽略标点符号
    bool enable_statistics;         // 启用统计
    size_t result_length;           // 结果长度
    size_t max_input_length;        // 最大输入长度
    bool use_phonetic_rules;        // 使用语音规则
} soundex_config_t;

typedef struct {
    soundex_error_t last_error;     // 最后一次错误
    size_t encodings;               // 编码计数
    size_t comparisons;             // 比较计数
    bool is_initialized;            // 是否初始化
    size_t average_input_length;    // 平均输入长度
    size_t average_output_length;   // 平均输出长度
} soundex_state_t;

/**
 * @brief 计算 Soundex 码 (结果存入 4 字节缓冲区)
 * @param s 输入字符串
 * @param res 输出缓冲区
 */
void soundex_encode(const char *s, char *res);

/**
 * @brief 增强版 Soundex 编码
 * @param s 输入字符串
 * @param res 输出缓冲区
 * @param res_size 输出缓冲区大小
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
soundex_error_t soundex_encode_ex(const char *s, char *res, size_t res_size,
                                 const soundex_config_t *config,
                                 soundex_state_t *state);

/**
 * @brief 计算两个字符串的 Soundex 相似度
 * @param s1 第一个字符串
 * @param s2 第二个字符串
 * @param similarity 输出相似度 (0.0-1.0)
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
soundex_error_t soundex_compare(const char *s1, const char *s2,
                               float *similarity,
                               const soundex_config_t *config,
                               soundex_state_t *state);

/**
 * @brief 批量编码字符串数组
 * @param strings 字符串数组
 * @param count 数组大小
 * @param results 结果数组
 * @param result_size 每个结果的大小
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
soundex_error_t soundex_batch_encode(const char **strings, size_t count,
                                    char **results, size_t result_size,
                                    const soundex_config_t *config,
                                    soundex_state_t *state);

/**
 * @brief 编码文件中的字符串
 * @param input_file 输入文件
 * @param output_file 输出文件
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
soundex_error_t soundex_encode_file(const char *input_file, const char *output_file,
                                   const soundex_config_t *config,
                                   soundex_state_t *state);

/**
 * @brief 计算 Metaphone 码
 * @param s 输入字符串
 * @param res 输出缓冲区
 * @param res_size 输出缓冲区大小
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
soundex_error_t soundex_metaphone(const char *s, char *res, size_t res_size,
                                 const soundex_config_t *config,
                                 soundex_state_t *state);

/**
 * @brief 计算 Double Metaphone 码
 * @param s 输入字符串
 * @param res1 主输出缓冲区
 * @param res2 备用输出缓冲区
 * @param res_size 输出缓冲区大小
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
soundex_error_t soundex_double_metaphone(const char *s, char *res1, char *res2,
                                         size_t res_size,
                                         const soundex_config_t *config,
                                         soundex_state_t *state);

/**
 * @brief 验证 Soundex 码
 * @param code Soundex 码
 * @param valid 输出验证结果
 * @param config 配置选项
 * @return 错误码
 */
soundex_error_t soundex_validate(const char *code, bool *valid,
                                const soundex_config_t *config);

/**
 * @brief 获取 Soundex 结果长度
 * @param config 配置选项
 * @return 结果长度
 */
size_t soundex_get_result_length(const soundex_config_t *config);

/**
 * @brief 获取 Soundex 算法名称
 * @param algorithm 算法类型
 * @return 算法名称字符串
 */
const char* soundex_get_algorithm_name(soundex_algorithm_t algorithm);

/**
 * @brief 获取 Soundex 统计信息
 * @param state 状态结构
 * @return 错误码
 */
soundex_error_t soundex_get_statistics(soundex_state_t *state);

/**
 * @brief 重置 Soundex 状态
 * @param state 状态结构
 */
void soundex_reset_state(soundex_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* soundex_strerror(const soundex_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void soundex_config_init(soundex_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void soundex_state_init(soundex_state_t *state);

#endif // C_UTILS_SOUNDEX_H
