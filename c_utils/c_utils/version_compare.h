#ifndef C_UTILS_VERSION_COMPARE_H
#define C_UTILS_VERSION_COMPARE_H

#include <stddef.h>
#include <stdbool.h>

// 版本比较错误码
typedef enum {
    VERSION_COMPARE_OK = 0,
    VERSION_COMPARE_INVALID_PARAMS = -1,
    VERSION_COMPARE_INVALID_FORMAT = -2,
    VERSION_COMPARE_MEMORY_ERROR = -3,
    VERSION_COMPARE_UNSUPPORTED_FORMAT = -4
} version_compare_error_t;

// 版本比较配置选项
typedef struct {
    bool strict_mode;           // 是否使用严格模式
    bool allow_pre_release;     // 是否允许预发布版本
    bool allow_build_metadata;  // 是否允许构建元数据
    bool use_semver_rules;      // 是否使用语义化版本规则
    size_t max_version_length;  // 最大版本字符串长度
} version_compare_config_t;

// 版本比较上下文
typedef struct {
    version_compare_config_t config;
    version_compare_error_t last_error;
    size_t compare_count;
    size_t parse_count;
} version_compare_ctx_t;

// 创建版本比较上下文
version_compare_error_t version_compare_create(version_compare_ctx_t** ctx, const version_compare_config_t* config);

// 销毁版本比较上下文
void version_compare_destroy(version_compare_ctx_t* ctx);

// 语义版本比较
// 返回 VERSION_COMPARE_OK 表示成功，其他值表示错误
// result 将包含比较结果: -1 (v1 < v2), 1 (v1 > v2), 0 (相等)
version_compare_error_t version_compare_str_safe(version_compare_ctx_t* ctx, const char *v1, const char *v2, int* result);

// 批量版本比较
// 返回 VERSION_COMPARE_OK 表示成功，其他值表示错误
version_compare_error_t version_compare_batch(version_compare_ctx_t* ctx, const char** versions, size_t count, int* results);

// 版本范围检查
// 返回 true 表示版本在范围内，false 表示不在范围内
bool version_compare_in_range(version_compare_ctx_t* ctx, const char* version, const char* range);

// 版本格式验证
// 返回 true 表示格式有效，false 表示无效
bool version_compare_validate(version_compare_ctx_t* ctx, const char* version);

// 版本排序
// 返回 VERSION_COMPARE_OK 表示成功，其他值表示错误
version_compare_error_t version_compare_sort(version_compare_ctx_t* ctx, char** versions, size_t count, bool ascending);

// 传统版本比较函数（向后兼容）
// 语义版本比较 (v1 < v2 返回 -1, v1 > v2 返回 1, 相等返回 0)
int version_compare_str(const char *v1, const char *v2);

// 获取最后一次错误
version_compare_error_t version_compare_get_last_error(version_compare_ctx_t* ctx);

// 获取错误信息
const char* version_compare_strerror(version_compare_error_t error);

#endif // C_UTILS_VERSION_COMPARE_H
