#ifndef C_UTILS_THREAD_LOCAL_STORAGE_H
#define C_UTILS_THREAD_LOCAL_STORAGE_H

#include <pthread.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef pthread_key_t tls_key_t;

typedef enum {
    TLS_OK = 0,
    TLS_ERROR_INVALID_PARAMS = -1,
    TLS_ERROR_KEY_CREATE = -2,
    TLS_ERROR_KEY_DELETE = -3,
    TLS_ERROR_SET_VALUE = -4,
    TLS_ERROR_GET_VALUE = -5,
    TLS_ERROR_MEMORY = -6,
    TLS_ERROR_DESTRUCTOR_NULL = -7,
    TLS_ERROR_KEY_NULL = -8,
    TLS_ERROR_VALUE_NULL = -9,
    TLS_ERROR_THREAD_NOT_FOUND = -10,
    TLS_ERROR_FILE_OPEN = -11,
    TLS_ERROR_FILE_READ = -12,
    TLS_ERROR_FILE_WRITE = -13,
    TLS_ERROR_BATCH_OPERATION = -14,
    TLS_ERROR_STATISTICS = -15
} tls_error_t;

typedef struct {
    bool enable_statistics;        // 启用统计
    bool enable_destructors;       // 启用析构函数
    bool enable_validation;        // 启用验证
    bool enable_thread_safety;     // 启用线程安全
    size_t max_keys;               // 最大键数量
    size_t initial_capacity;       // 初始容量
    bool use_malloc;               // 使用malloc分配内存
    bool auto_cleanup;             // 自动清理
} tls_config_t;

typedef struct {
    tls_error_t last_error;         // 最后一次错误
    size_t key_creations;           // 键创建次数
    size_t key_deletions;           // 键删除次数
    size_t value_sets;              // 值设置次数
    size_t value_gets;              // 值获取次数
    bool is_initialized;            // 是否初始化
    size_t active_keys;              // 活跃键数量
    size_t active_threads;           // 活跃线程数量
} tls_state_t;

/**
 * @brief 创建TLS键
 * @param key 输出键
 * @param destructor 析构函数
 * @return 成功返回0，失败返回错误码
 */
int tls_key_create(tls_key_t *key, void (*destructor)(void*));

/**
 * @brief 增强版TLS键创建
 * @param key 输出键
 * @param destructor 析构函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_key_create_ex(tls_key_t *key, void (*destructor)(void*),
                             const tls_config_t *config,
                             tls_state_t *state);

/**
 * @brief 设置TLS值
 * @param key 键
 * @param value 值
 * @return 成功返回0，失败返回错误码
 */
int tls_set_value(tls_key_t key, const void *value);

/**
 * @brief 增强版TLS值设置
 * @param key 键
 * @param value 值
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_set_value_ex(tls_key_t key, const void *value,
                            const tls_config_t *config,
                            tls_state_t *state);

/**
 * @brief 获取TLS值
 * @param key 键
 * @return 值，失败返回NULL
 */
void* tls_get_value(tls_key_t key);

/**
 * @brief 增强版TLS值获取
 * @param key 键
 * @param value 输出值
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_get_value_ex(tls_key_t key, void **value,
                            const tls_config_t *config,
                            tls_state_t *state);

/**
 * @brief 删除TLS键
 * @param key 键
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_key_delete(tls_key_t key, tls_state_t *state);

/**
 * @brief 批量创建TLS键
 * @param keys 输出键数组
 * @param count 键数量
 * @param destructor 析构函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_batch_key_create(tls_key_t *keys, size_t count,
                               void (*destructor)(void*),
                               const tls_config_t *config,
                               tls_state_t *state);

/**
 * @brief 批量删除TLS键
 * @param keys 键数组
 * @param count 键数量
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_batch_key_delete(const tls_key_t *keys, size_t count,
                               tls_state_t *state);

/**
 * @brief 批量设置TLS值
 * @param keys 键数组
 * @param values 值数组
 * @param count 数量
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_batch_set_value(const tls_key_t *keys, const void **values,
                               size_t count,
                               const tls_config_t *config,
                               tls_state_t *state);

/**
 * @brief 批量获取TLS值
 * @param keys 键数组
 * @param values 输出值数组
 * @param count 数量
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_batch_get_value(const tls_key_t *keys, void **values,
                               size_t count,
                               const tls_config_t *config,
                               tls_state_t *state);

/**
 * @brief 验证TLS键
 * @param key 键
 * @param valid 输出验证结果
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_validate_key(tls_key_t key, bool *valid,
                            tls_state_t *state);

/**
 * @brief 获取当前线程ID
 * @param thread_id 输出线程ID
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_get_thread_id(uint64_t *thread_id,
                             tls_state_t *state);

/**
 * @brief 清理当前线程的TLS值
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_cleanup_thread(tls_state_t *state);

/**
 * @brief 从文件加载TLS配置
 * @param filename 文件名
 * @param config 输出配置
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_load_config(const char *filename,
                           tls_config_t *config,
                           tls_state_t *state);

/**
 * @brief 保存TLS配置到文件
 * @param filename 文件名
 * @param config 配置
 * @param state 状态输出
 * @return 错误码
 */
tls_error_t tls_save_config(const char *filename,
                           const tls_config_t *config,
                           tls_state_t *state);

/**
 * @brief 保存TLS状态到文件
 * @param filename 文件名
 * @param state 状态
 * @return 错误码
 */
tls_error_t tls_save_state(const char *filename,
                          const tls_state_t *state);

/**
 * @brief 获取TLS统计信息
 * @param state 状态
 * @return 错误码
 */
tls_error_t tls_get_statistics(tls_state_t *state);

/**
 * @brief 重置TLS状态
 * @param state 状态
 */
void tls_reset_state(tls_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* tls_strerror(const tls_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置
 */
void tls_config_init(tls_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态
 */
void tls_state_init(tls_state_t *state);

#endif // C_UTILS_THREAD_LOCAL_STORAGE_H
