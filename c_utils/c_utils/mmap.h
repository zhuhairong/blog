#ifndef C_UTILS_MMAP_H
#define C_UTILS_MMAP_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief 内存映射错误码
 */
typedef enum {
    MMAP_OK = 0,              /**< 成功 */
    MMAP_ERROR_NULL_PTR,      /**< 空指针错误 */
    MMAP_ERROR_OPEN_FILE,     /**< 文件打开失败 */
    MMAP_ERROR_FILE_SIZE,     /**< 文件大小获取失败 */
    MMAP_ERROR_MMAP,          /**< 内存映射失败 */
    MMAP_ERROR_INVALID_ARGS,  /**< 无效参数 */
    MMAP_ERROR_UNMAP,         /**< 解除映射失败 */
    MMAP_ERROR_MAX            /**< 最大错误码 */
} mmap_error_t;

/**
 * @brief 内存映射配置
 */
typedef struct {
    bool read_only;           /**< 是否只读映射 */
    bool private;             /**< 是否私有映射 */
    bool populate;            /**< 是否预加载到内存 */
    size_t offset;            /**< 文件偏移量 */
    size_t length;            /**< 映射长度 (0 表示整个文件) */
} mmap_config_t;

/**
 * @brief 内存映射信息
 */
typedef struct {
    void *addr;               /**< 映射地址 */
    size_t size;              /**< 映射大小 */
    size_t file_size;         /**< 文件大小 */
    bool read_only;           /**< 是否只读 */
    bool private;             /**< 是否私有 */
    const char *filename;     /**< 文件名 */
} mmap_info_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
mmap_config_t mmap_default_config(void);

/**
 * @brief 映射文件到内存
 * @param filename 文件名
 * @param size 映射大小输出
 * @return 映射地址，失败返回 NULL
 */
void* mmap_file(const char *filename, size_t *size);

/**
 * @brief 使用自定义配置映射文件到内存
 * @param filename 文件名
 * @param size 映射大小输出
 * @param config 配置选项
 * @param error 错误码输出
 * @return 映射地址，失败返回 NULL
 */
void* mmap_file_ex(const char *filename, size_t *size, const mmap_config_t *config, mmap_error_t *error);

/**
 * @brief 映射文件到内存（返回详细信息）
 * @param filename 文件名
 * @param config 配置选项
 * @param info 映射信息输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool mmap_file_info(const char *filename, const mmap_config_t *config, mmap_info_t *info, mmap_error_t *error);

/**
 * @brief 解除映射
 * @param ptr 映射地址
 * @param size 映射大小
 */
void mmap_free(void *ptr, size_t size);

/**
 * @brief 解除映射（带错误码）
 * @param ptr 映射地址
 * @param size 映射大小
 * @param error 错误码输出
 * @return 是否成功
 */
bool mmap_free_ex(void *ptr, size_t size, mmap_error_t *error);

/**
 * @brief 同步映射内存到文件
 * @param ptr 映射地址
 * @param size 同步大小
 * @param error 错误码输出
 * @return 是否成功
 */
bool mmap_sync(void *ptr, size_t size, mmap_error_t *error);

/**
 * @brief 创建并映射匿名内存
 * @param size 映射大小
 * @param read_only 是否只读
 * @param error 错误码输出
 * @return 映射地址，失败返回 NULL
 */
void* mmap_anonymous(size_t size, bool read_only, mmap_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* mmap_error_string(mmap_error_t error);

#endif // C_UTILS_MMAP_H
