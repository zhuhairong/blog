#ifndef C_UTILS_ENV_H
#define C_UTILS_ENV_H

#include <stdbool.h>
#include <stddef.h>

// 环境变量错误码
typedef enum {
    ENV_OK = 0,
    ENV_ERROR_INVALID_PARAM,
    ENV_ERROR_SET_FAILED,
    ENV_ERROR_UNSET_FAILED,
    ENV_ERROR_MEMORY_ALLOC,
    ENV_ERROR_BUFFER_TOO_SMALL,
    ENV_ERROR_PLATFORM_UNSUPPORTED
} env_error_t;

// 环境变量选项
typedef struct {
    bool expand_variables;
    bool trim_whitespace;
    bool case_insensitive;
    bool thread_safe;
} env_options_t;

// 默认环境变量选项
// 返回: 默认选项
env_options_t env_default_options(void);

// 获取环境变量值
// name: 环境变量名称
// default_val: 默认值
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 环境变量值或默认值
const char* env_get(const char *name, const char *default_val, const env_options_t *options, env_error_t *error);

// 获取整数环境变量
// name: 环境变量名称
// default_val: 默认值
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 整数环境变量值或默认值
int env_get_int(const char *name, int default_val, const env_options_t *options, env_error_t *error);

// 获取浮点数环境变量
// name: 环境变量名称
// default_val: 默认值
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 浮点数环境变量值或默认值
double env_get_double(const char *name, double default_val, const env_options_t *options, env_error_t *error);

// 获取布尔值环境变量
// name: 环境变量名称
// default_val: 默认值
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 布尔值环境变量值或默认值
bool env_get_bool(const char *name, bool default_val, const env_options_t *options, env_error_t *error);

// 设置环境变量
// name: 环境变量名称
// value: 环境变量值
// overwrite: 是否覆盖已存在的环境变量
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool env_set(const char *name, const char *value, bool overwrite, const env_options_t *options, env_error_t *error);

// 删除环境变量
// name: 环境变量名称
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool env_unset(const char *name, const env_options_t *options, env_error_t *error);

// 检查环境变量是否存在
// name: 环境变量名称
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 存在返回 true，不存在返回 false
bool env_has(const char *name, const env_options_t *options, env_error_t *error);

// 展开环境变量
// str: 包含环境变量的字符串
// out: 输出缓冲区
// out_size: 输出缓冲区大小
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool env_expand(const char *str, char *out, size_t out_size, const env_options_t *options, env_error_t *error);

// 获取所有环境变量
// envp: 环境变量数组（输出参数，格式为 "NAME=VALUE"）
// count: 环境变量数量（输出参数）
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool env_get_all(char ***envp, size_t *count, const env_options_t *options, env_error_t *error);

// 释放环境变量数组
// envp: 环境变量数组
// count: 环境变量数量
void env_free_all(char ***envp, size_t count);

// 批量设置环境变量
// envp: 环境变量数组（格式为 "NAME=VALUE"）
// count: 环境变量数量
// overwrite: 是否覆盖已存在的环境变量
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool env_set_all(const char **envp, size_t count, bool overwrite, const env_options_t *options, env_error_t *error);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* env_strerror(env_error_t error);

#endif // C_UTILS_ENV_H
