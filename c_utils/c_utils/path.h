#ifndef C_UTILS_PATH_H
#define C_UTILS_PATH_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief 路径处理错误码
 */
typedef enum {
    PATH_OK = 0,                  /**< 成功 */
    PATH_ERROR_NULL_PTR,          /**< 空指针错误 */
    PATH_ERROR_INVALID_PATH,      /**< 无效路径 */
    PATH_ERROR_MEMORY_ALLOC,      /**< 内存分配失败 */
    PATH_ERROR_BUFFER_TOO_SMALL,  /**< 缓冲区太小 */
    PATH_ERROR_MAX                /**< 最大错误码 */
} path_error_t;

/**
 * @brief 路径配置
 */
typedef struct {
    bool normalize;               /**< 是否规范化路径 */
    bool resolve_symlinks;        /**< 是否解析符号链接 */
    bool case_sensitive;          /**< 是否大小写敏感 */
    char separator;               /**< 路径分隔符 ('/' 或 '\') */
} path_config_t;

/**
 * @brief 路径信息
 */
typedef struct {
    char *directory;              /**< 目录部分 */
    char *filename;               /**< 文件名部分 */
    char *extension;              /**< 文件扩展名 */
    bool is_absolute;             /**< 是否为绝对路径 */
    bool is_directory;            /**< 是否为目录 */
    bool exists;                  /**< 是否存在 */
} path_info_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
path_config_t path_default_config(void);

/**
 * @brief 获取系统路径分隔符
 * @return 路径分隔符
 */
char path_get_separator(void);

/**
 * @brief 路径拼接
 * @param p1 第一个路径
 * @param p2 第二个路径
 * @return 拼接后的路径，失败返回 NULL
 */
char* path_join(const char *p1, const char *p2);

/**
 * @brief 路径拼接（使用自定义配置）
 * @param p1 第一个路径
 * @param p2 第二个路径
 * @param config 配置选项
 * @param error 错误码输出
 * @return 拼接后的路径，失败返回 NULL
 */
char* path_join_ex(const char *p1, const char *p2, const path_config_t *config, path_error_t *error);

/**
 * @brief 多路径拼接
 * @param paths 路径数组
 * @param count 路径数量
 * @param error 错误码输出
 * @return 拼接后的路径，失败返回 NULL
 */
char* path_join_multiple(const char **paths, size_t count, path_error_t *error);

/**
 * @brief 获取目录名
 * @param path 路径
 * @return 目录名，失败返回 NULL
 */
char* path_dirname(const char *path);

/**
 * @brief 获取目录名（使用自定义配置）
 * @param path 路径
 * @param config 配置选项
 * @param error 错误码输出
 * @return 目录名，失败返回 NULL
 */
char* path_dirname_ex(const char *path, const path_config_t *config, path_error_t *error);

/**
 * @brief 获取文件名
 * @param path 路径
 * @return 文件名
 */
const char* path_basename(const char *path);

/**
 * @brief 获取文件名（不包含扩展名）
 * @param path 路径
 * @param error 错误码输出
 * @return 文件名（不包含扩展名），失败返回 NULL
 */
char* path_basename_no_ext(const char *path, path_error_t *error);

/**
 * @brief 检查是否为绝对路径
 * @param path 路径
 * @return 是否为绝对路径
 */
bool path_is_absolute(const char *path);

/**
 * @brief 检查是否为相对路径
 * @param path 路径
 * @return 是否为相对路径
 */
bool path_is_relative(const char *path);

/**
 * @brief 规范化路径
 * @param path 路径
 * @param error 错误码输出
 * @return 规范化后的路径，失败返回 NULL
 */
char* path_normalize(const char *path, path_error_t *error);

/**
 * @brief 解析路径信息
 * @param path 路径
 * @param info 路径信息输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool path_parse(const char *path, path_info_t *info, path_error_t *error);

/**
 * @brief 释放路径信息
 * @param info 路径信息
 */
void path_free_info(path_info_t *info);

/**
 * @brief 获取文件扩展名
 * @param path 路径
 * @return 文件扩展名（包含点号），无扩展名返回空字符串
 */
const char* path_get_extension(const char *path);

/**
 * @brief 添加文件扩展名
 * @param path 路径
 * @param extension 扩展名
 * @param error 错误码输出
 * @return 带扩展名的路径，失败返回 NULL
 */
char* path_add_extension(const char *path, const char *extension, path_error_t *error);

/**
 * @brief 移除文件扩展名
 * @param path 路径
 * @param error 错误码输出
 * @return 无扩展名的路径，失败返回 NULL
 */
char* path_remove_extension(const char *path, path_error_t *error);

/**
 * @brief 检查路径是否存在
 * @param path 路径
 * @return 是否存在
 */
bool path_exists(const char *path);

/**
 * @brief 检查路径是否为目录
 * @param path 路径
 * @return 是否为目录
 */
bool path_is_directory(const char *path);

/**
 * @brief 检查路径是否为文件
 * @param path 路径
 * @return 是否为文件
 */
bool path_is_file(const char *path);

/**
 * @brief 确保目录存在
 * @param path 路径
 * @param error 错误码输出
 * @return 是否成功
 */
bool path_mkdir_p(const char *path, path_error_t *error);

/**
 * @brief 获取绝对路径
 * @param path 路径
 * @param error 错误码输出
 * @return 绝对路径，失败返回 NULL
 */
char* path_absolute(const char *path, path_error_t *error);

/**
 * @brief 获取相对路径
 * @param from 起始路径
 * @param to 目标路径
 * @param error 错误码输出
 * @return 相对路径，失败返回 NULL
 */
char* path_relative(const char *from, const char *to, path_error_t *error);

/**
 * @brief 比较两个路径
 * @param path1 第一个路径
 * @param path2 第二个路径
 * @param case_sensitive 是否大小写敏感
 * @return 0 表示相等，负数表示 path1 < path2，正数表示 path1 > path2
 */
int path_compare(const char *path1, const char *path2, bool case_sensitive);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* path_error_string(path_error_t error);

#endif // C_UTILS_PATH_H
