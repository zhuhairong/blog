#ifndef C_UTILS_VERSION_H
#define C_UTILS_VERSION_H

#include <stddef.h>
#include <stdbool.h>

// 版本错误码
typedef enum {
    VERSION_OK = 0,
    VERSION_INVALID_PARAMS = -1,
    VERSION_INVALID_FORMAT = -2,
    VERSION_OUT_OF_RANGE = -3,
    VERSION_MEMORY_ERROR = -4,
    VERSION_COMPARE_ERROR = -5
} version_error_t;

// 版本配置选项
typedef struct {
    bool allow_pre_release;     // 是否允许预发布版本
    bool allow_build_metadata;  // 是否允许构建元数据
    bool strict_mode;           // 是否使用严格模式
    size_t max_version_length;  // 最大版本字符串长度
} version_config_t;

// 版本结构体
typedef struct {
    int major;
    int minor;
    int patch;
    char *pre_release;
    char *build_metadata;
} version_t;

// 版本上下文
typedef struct {
    version_config_t config;
    version_error_t last_error;
    size_t parse_count;
    size_t compare_count;
} version_ctx_t;

// 创建版本上下文
version_error_t version_create(version_ctx_t** ctx, const version_config_t* config);

// 销毁版本上下文
void version_destroy(version_ctx_t* ctx);

// 解析版本字符串 (如 "1.2.3" 或 "1.2.3-alpha.1+build.1")
// 返回 VERSION_OK 表示成功，其他值表示错误
version_error_t version_parse(version_ctx_t* ctx, const char *v_str, version_t* out);

// 比较版本
// 返回 1 (v1 > v2), -1 (v1 < v2), 0 (相等)
int version_compare(version_t v1, version_t v2);

// 版本比较（带错误处理）
// 返回 VERSION_OK 表示成功，其他值表示错误
// result 将包含比较结果
version_error_t version_compare_safe(version_ctx_t* ctx, version_t v1, version_t v2, int* result);

// 版本转换为字符串
// 返回 VERSION_OK 表示成功，其他值表示错误
version_error_t version_to_string(version_ctx_t* ctx, const version_t* v, char* out, size_t out_size);

// 版本增量
// 返回 VERSION_OK 表示成功，其他值表示错误
version_error_t version_increment(version_ctx_t* ctx, version_t* v, int part);

// 检查版本是否有效
// 返回 true 表示有效，false 表示无效
bool version_is_valid(const version_t* v);

// 复制版本
// 返回 VERSION_OK 表示成功，其他值表示错误
version_error_t version_copy(version_t* dest, const version_t* src);

// 销毁版本
void version_free(version_t* v);



// 获取最后一次错误
version_error_t version_get_last_error(version_ctx_t* ctx);

// 获取错误信息
const char* version_strerror(version_error_t error);

#endif // C_UTILS_VERSION_H
