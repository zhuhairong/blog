#ifndef C_UTILS_DYNAMIC_LIB_H
#define C_UTILS_DYNAMIC_LIB_H

#include <stddef.h>
#include <stdbool.h>

// 动态库错误码
typedef enum {
    DYNAMIC_LIB_OK = 0,
    DYNAMIC_LIB_ERROR_INVALID_PARAM,
    DYNAMIC_LIB_ERROR_OPEN_FAILED,
    DYNAMIC_LIB_ERROR_SYMBOL_NOT_FOUND,
    DYNAMIC_LIB_ERROR_CLOSE_FAILED,
    DYNAMIC_LIB_ERROR_PLATFORM_UNSUPPORTED,
    DYNAMIC_LIB_ERROR_MEMORY_ALLOC
} dynamic_lib_error_t;

// 动态库句柄
typedef struct {
    void *handle;
    char *path;
    bool has_error;
    dynamic_lib_error_t error;
    char error_msg[256];
} dynamic_lib_t;

// 打开动态库
// path: 动态库路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回动态库指针，失败返回 NULL
dynamic_lib_t* dynamic_lib_open(const char *path, dynamic_lib_error_t *error);

// 关闭动态库
// lib: 动态库
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dynamic_lib_close(dynamic_lib_t *lib, dynamic_lib_error_t *error);

// 获取符号
// lib: 动态库
// name: 符号名称
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回符号指针，失败返回 NULL
void* dynamic_lib_get_symbol(dynamic_lib_t *lib, const char *name, dynamic_lib_error_t *error);

// 检查动态库是否有效
// lib: 动态库
// 返回: 有效返回 true，无效返回 false
bool dynamic_lib_is_valid(const dynamic_lib_t *lib);

// 获取动态库路径
// lib: 动态库
// 返回: 动态库路径，失败返回 NULL
const char* dynamic_lib_get_path(const dynamic_lib_t *lib);

// 检查动态库是否有错误
// lib: 动态库
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool dynamic_lib_has_error(const dynamic_lib_t *lib, dynamic_lib_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* dynamic_lib_strerror(dynamic_lib_error_t error);

#endif // C_UTILS_DYNAMIC_LIB_H
