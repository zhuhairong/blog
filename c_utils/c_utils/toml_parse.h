#ifndef C_UTILS_TOML_PARSE_H
#define C_UTILS_TOML_PARSE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TOML_OK = 0,
    TOML_ERROR_INVALID_PARAMS = -1,
    TOML_ERROR_PARSE = -2,
    TOML_ERROR_GET = -3,
    TOML_ERROR_SET = -4,
    TOML_ERROR_DELETE = -5,
    TOML_ERROR_MEMORY = -6,
    TOML_ERROR_KEY_NOT_FOUND = -7,
    TOML_ERROR_TYPE_MISMATCH = -8,
    TOML_ERROR_FILE_OPEN = -9,
    TOML_ERROR_FILE_READ = -10,
    TOML_ERROR_FILE_WRITE = -11,
    TOML_ERROR_SYNTAX = -12,
    TOML_ERROR_SEMANTIC = -13,
    TOML_ERROR_NESTED_TOO_DEEP = -14,
    TOML_ERROR_STRING_TOO_LONG = -15,
    TOML_ERROR_STATISTICS = -16,
    TOML_ERROR_BATCH_OPERATION = -17
} toml_error_t;

typedef enum {
    TOML_TYPE_STRING = 0,
    TOML_TYPE_INTEGER,
    TOML_TYPE_FLOAT,
    TOML_TYPE_BOOL,
    TOML_TYPE_ARRAY,
    TOML_TYPE_TABLE,
    TOML_TYPE_DATE_TIME,
    TOML_TYPE_NONE
} toml_type_t;

typedef struct {
    bool enable_statistics;        // 启用统计
    bool enable_validation;        // 启用验证
    bool enable_nested;            // 启用嵌套
    bool enable_arrays;            // 启用数组
    bool enable_tables;            // 启用表
    bool enable_date_time;         // 启用日期时间
    bool strict_mode;              // 严格模式
    size_t max_key_length;         // 最大键长度
    size_t max_value_length;       // 最大值长度
    size_t max_nesting_depth;      // 最大嵌套深度
    size_t buffer_size;            // 缓冲区大小
} toml_config_t;

typedef struct {
    toml_error_t last_error;       // 最后一次错误
    size_t parse_calls;            // 解析调用次数
    size_t get_calls;              // 获取调用次数
    size_t set_calls;              // 设置调用次数
    size_t delete_calls;           // 删除调用次数
    bool is_initialized;           // 是否初始化
    size_t keys_parsed;            // 解析的键数量
    size_t tables_parsed;          // 解析的表数量
    size_t arrays_parsed;          // 解析的数组数量
} toml_state_t;

/**
 * @brief TOML 值结构体
 */
typedef struct {
    toml_type_t type;
    union {
        char *string;
        int64_t integer;
        double floating;
        bool boolean;
        void *array;
        void *table;
        char *date_time;
    } value;
    size_t length;                 // 数组长度或字符串长度
    bool is_valid;                // 是否有效
} toml_value_t;

/**
 * @brief 极简 TOML 键值提取 (仅支持 key = "value" 格式)
 * @param toml TOML字符串
 * @param key 键名
 * @param out 输出缓冲区
 * @param max_len 最大长度
 * @return 指向输出缓冲区的指针，失败返回NULL
 */
const char* toml_get_string(const char *toml, const char *key, char *out, int max_len);

/**
 * @brief 增强版TOML字符串获取
 * @param toml TOML字符串
 * @param key 键名
 * @param out 输出缓冲区
 * @param max_len 最大长度
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_get_string_ex(const char *toml, const char *key, char *out, size_t max_len,
                              const toml_config_t *config,
                              toml_state_t *state);

/**
 * @brief 获取TOML整数值
 * @param toml TOML字符串
 * @param key 键名
 * @param value 输出值
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_get_integer(const char *toml, const char *key, int64_t *value,
                             const toml_config_t *config,
                             toml_state_t *state);

/**
 * @brief 获取TOML浮点数值
 * @param toml TOML字符串
 * @param key 键名
 * @param value 输出值
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_get_float(const char *toml, const char *key, double *value,
                          const toml_config_t *config,
                          toml_state_t *state);

/**
 * @brief 获取TOML布尔值
 * @param toml TOML字符串
 * @param key 键名
 * @param value 输出值
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_get_bool(const char *toml, const char *key, bool *value,
                          const toml_config_t *config,
                          toml_state_t *state);

/**
 * @brief 获取TOML值
 * @param toml TOML字符串
 * @param key 键名
 * @param value 输出值
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_get_value(const char *toml, const char *key, toml_value_t *value,
                          const toml_config_t *config,
                          toml_state_t *state);

/**
 * @brief 设置TOML值
 * @param toml TOML字符串
 * @param key 键名
 * @param value 值
 * @param type 值类型
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_set_value(char *toml, const char *key, const void *value,
                          toml_type_t type,
                          const toml_config_t *config,
                          toml_state_t *state);

/**
 * @brief 删除TOML键
 * @param toml TOML字符串
 * @param key 键名
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_delete_key(char *toml, const char *key,
                           const toml_config_t *config,
                           toml_state_t *state);

/**
 * @brief 解析TOML文件
 * @param filename 文件名
 * @param config 配置选项
 * @param state 状态输出
 * @return 解析后的TOML字符串，失败返回NULL
 */
char* toml_parse_file(const char *filename,
                     const toml_config_t *config,
                     toml_state_t *state);

/**
 * @brief 保存TOML到文件
 * @param filename 文件名
 * @param toml TOML字符串
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_save_file(const char *filename, const char *toml,
                          toml_state_t *state);

/**
 * @brief 验证TOML字符串
 * @param toml TOML字符串
 * @param valid 输出验证结果
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_validate(const char *toml, bool *valid,
                         const toml_config_t *config,
                         toml_state_t *state);

/**
 * @brief 格式化TOML字符串
 * @param toml TOML字符串
 * @param formatted 输出格式化后的字符串
 * @param max_len 最大长度
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_format(const char *toml, char *formatted, size_t max_len,
                        const toml_config_t *config,
                        toml_state_t *state);

/**
 * @brief 合并两个TOML字符串
 * @param toml1 第一个TOML字符串
 * @param toml2 第二个TOML字符串
 * @param merged 输出合并后的字符串
 * @param max_len 最大长度
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_merge(const char *toml1, const char *toml2, char *merged, size_t max_len,
                       const toml_config_t *config,
                       toml_state_t *state);

/**
 * @brief 获取TOML键数量
 * @param toml TOML字符串
 * @param count 输出键数量
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_get_key_count(const char *toml, size_t *count,
                              const toml_config_t *config,
                              toml_state_t *state);

/**
 * @brief 列出TOML键
 * @param toml TOML字符串
 * @param keys 输出键数组
 * @param max_keys 最大键数量
 * @param key_count 输出键数量
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_list_keys(const char *toml, char **keys, size_t max_keys,
                          size_t *key_count,
                          const toml_config_t *config,
                          toml_state_t *state);

/**
 * @brief 从文件加载TOML配置
 * @param filename 文件名
 * @param config 输出配置
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_load_config(const char *filename,
                             toml_config_t *config,
                             toml_state_t *state);

/**
 * @brief 保存TOML配置到文件
 * @param filename 文件名
 * @param config 配置
 * @param state 状态输出
 * @return 错误码
 */
toml_error_t toml_save_config(const char *filename,
                             const toml_config_t *config,
                             toml_state_t *state);

/**
 * @brief 释放TOML值
 * @param value TOML值
 * @return 错误码
 */
toml_error_t toml_free_value(toml_value_t *value);

/**
 * @brief 获取TOML统计信息
 * @param state 状态
 * @return 错误码
 */
toml_error_t toml_get_statistics(toml_state_t *state);

/**
 * @brief 重置TOML状态
 * @param state 状态
 */
void toml_reset_state(toml_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* toml_strerror(const toml_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置
 */
void toml_config_init(toml_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态
 */
void toml_state_init(toml_state_t *state);

#endif // C_UTILS_TOML_PARSE_H
