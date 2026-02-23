#ifndef C_UTILS_SHM_H
#define C_UTILS_SHM_H

#include <stddef.h>
#include <stdbool.h>
#include <sys/stat.h>

typedef enum {
    SHM_OK = 0,
    SHM_ERROR_INVALID_PARAMS = -1,
    SHM_ERROR_NAME_NULL = -2,
    SHM_ERROR_SIZE_ZERO = -3,
    SHM_ERROR_OPEN = -4,
    SHM_ERROR_FTRUNCATE = -5,
    SHM_ERROR_MAP = -6,
    SHM_ERROR_UNMAP = -7,
    SHM_ERROR_CLOSE = -8,
    SHM_ERROR_UNLINK = -9,
    SHM_ERROR_PERMISSION = -10,
    SHM_ERROR_EXISTS = -11,
    SHM_ERROR_NOT_FOUND = -12,
    SHM_ERROR_MEMORY = -13
} shm_error_t;

typedef struct {
    bool create_if_not_exists;  // 如果不存在则创建
    bool exclusive;             // 独占模式
    bool unlink_on_close;       // 关闭时自动删除
    bool read_only;             // 只读模式
    mode_t permissions;         // 权限模式
    size_t min_size;            // 最小大小
    size_t max_size;            // 最大大小（0表示无限制）
} shm_config_t;

typedef struct {
    shm_error_t last_error;     // 最后一次错误
    int error_code;             // 系统错误码
    size_t actual_size;         // 实际大小
    bool is_mapped;             // 是否已映射
    bool is_read_only;          // 是否为只读
    bool is_created;            // 是否为新创建
} shm_state_t;

/**
 * @brief 创建/打开共享内存
 * @param name 共享内存名称
 * @param size 大小
 * @return 共享内存指针，失败返回NULL
 */
void* shm_open_map(const char *name, size_t size);

/**
 * @brief 增强版共享内存创建/打开
 * @param name 共享内存名称
 * @param size 大小
 * @param config 配置选项
 * @param state 状态输出
 * @return 共享内存指针，失败返回NULL
 */
void* shm_open_map_ex(const char *name, size_t size, const shm_config_t *config, shm_state_t *state);

/**
 * @brief 解除映射并关闭共享内存
 * @param ptr 共享内存指针
 * @param name 共享内存名称
 * @param size 大小
 */
void shm_close_unmap(void *ptr, const char *name, size_t size);

/**
 * @brief 增强版解除映射并关闭共享内存
 * @param ptr 共享内存指针
 * @param name 共享内存名称
 * @param size 大小
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
shm_error_t shm_close_unmap_ex(void *ptr, const char *name, size_t size, const shm_config_t *config, shm_state_t *state);

/**
 * @brief 创建新的共享内存
 * @param name 共享内存名称
 * @param size 大小
 * @param permissions 权限模式
 * @param state 状态输出
 * @return 共享内存指针，失败返回NULL
 */
void* shm_create(const char *name, size_t size, mode_t permissions, shm_state_t *state);

/**
 * @brief 打开已存在的共享内存
 * @param name 共享内存名称
 * @param read_only 是否只读
 * @param state 状态输出
 * @return 共享内存指针，失败返回NULL
 */
void* shm_open_ex2(const char *name, bool read_only, shm_state_t *state);

/**
 * @brief 调整共享内存大小
 * @param name 共享内存名称
 * @param new_size 新大小
 * @param state 状态输出
 * @return 错误码
 */
shm_error_t shm_resize(const char *name, size_t new_size, shm_state_t *state);

/**
 * @brief 获取共享内存大小
 * @param name 共享内存名称
 * @param size 输出大小
 * @param state 状态输出
 * @return 错误码
 */
shm_error_t shm_get_size(const char *name, size_t *size, shm_state_t *state);

/**
 * @brief 删除共享内存
 * @param name 共享内存名称
 * @param state 状态输出
 * @return 错误码
 */
shm_error_t shm_delete(const char *name, shm_state_t *state);

/**
 * @brief 检查共享内存是否存在
 * @param name 共享内存名称
 * @param exists 输出结果
 * @return 错误码
 */
shm_error_t shm_exists(const char *name, bool *exists);

/**
 * @brief 锁定共享内存
 * @param ptr 共享内存指针
 * @param size 大小
 * @param state 状态输出
 * @return 错误码
 */
shm_error_t shm_lock(void *ptr, size_t size, shm_state_t *state);

/**
 * @brief 解锁共享内存
 * @param ptr 共享内存指针
 * @param size 大小
 * @param state 状态输出
 * @return 错误码
 */
shm_error_t shm_unlock(void *ptr, size_t size, shm_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* shm_strerror(const shm_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void shm_config_init(shm_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void shm_state_init(shm_state_t *state);

#endif // C_UTILS_SHM_H
