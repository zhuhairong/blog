#ifndef C_UTILS_UUID_H
#define C_UTILS_UUID_H

#include <stddef.h>
#include <stdbool.h>

// UUID 字符串长度 (36 字符 + \0)
#define UUID_STR_LEN 37

// UUID 二进制长度
#define UUID_BIN_LEN 16

// UUID 错误码
typedef enum {
    UUID_OK = 0,
    UUID_INVALID_PARAMS = -1,
    UUID_INVALID_FORMAT = -2,
    UUID_INVALID_VERSION = -3,
    UUID_MEMORY_ERROR = -4,
    UUID_GENERATION_ERROR = -5
} uuid_error_t;

// UUID 配置选项
typedef struct {
    bool use_hyphens;          // 是否使用连字符
    bool uppercase;            // 是否使用大写字母
    bool strict_validation;    // 是否使用严格验证
    size_t max_string_length;  // 最大字符串长度
} uuid_config_t;

// UUID 结构体
typedef struct {
    unsigned char data[UUID_BIN_LEN];
} uuid_t;

// UUID 上下文
typedef struct {
    uuid_config_t config;
    uuid_error_t last_error;
} uuid_ctx_t;

// 创建 UUID 上下文
uuid_error_t uuid_create(uuid_ctx_t** ctx, const uuid_config_t* config);

// 销毁 UUID 上下文
void uuid_destroy(uuid_ctx_t* ctx);

// 生成 UUID v4
// 返回 UUID_OK 表示成功，其他值表示错误
uuid_error_t uuid_generate_v4(uuid_ctx_t* ctx, uuid_t* uuid);

// 生成 UUID v4 字符串
// 返回 UUID_OK 表示成功，其他值表示错误
uuid_error_t uuid_generate_v4_string(uuid_ctx_t* ctx, char* out);

// 解析 UUID 字符串
// 返回 UUID_OK 表示成功，其他值表示错误
uuid_error_t uuid_parse(uuid_ctx_t* ctx, const char* str, uuid_t* uuid);

// UUID 转换为字符串
// 返回 UUID_OK 表示成功，其他值表示错误
uuid_error_t uuid_to_string(uuid_ctx_t* ctx, const uuid_t* uuid, char* out);

// 验证 UUID 字符串
// 返回 true 表示有效，false 表示无效
bool uuid_is_valid(const char* str);

// 获取 UUID 版本
// 返回版本号，0 表示无效
int uuid_get_version(const uuid_t* uuid);

// 比较两个 UUID
// 返回 0 表示相等，-1 表示 uuid1 < uuid2，1 表示 uuid1 > uuid2
int uuid_compare(const uuid_t* uuid1, const uuid_t* uuid2);

// 复制 UUID
void uuid_copy(uuid_t* dest, const uuid_t* src);

// 检查 UUID 是否为 nil
// 返回 true 表示是 nil UUID，false 表示不是
bool uuid_is_nil(const uuid_t* uuid);

// 传统 UUID v4 生成函数（向后兼容）
void uuid_v4(char *out);

// 获取最后一次错误
uuid_error_t uuid_get_last_error(uuid_ctx_t* ctx);

// 获取错误信息
const char* uuid_strerror(uuid_error_t error);

#endif // C_UTILS_UUID_H
