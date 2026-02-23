#ifndef C_UTILS_SKIPLIST_H
#define C_UTILS_SKIPLIST_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define SKIPLIST_MAX_LEVEL 16

typedef struct skiplist_node_s {
    void *key;
    void *value;
    struct skiplist_node_s *forward[1];
} skiplist_node_t;

typedef struct {
    skiplist_node_t *header;
    int level;
    size_t size;
    int (*compar)(const void *, const void *);
} skiplist_t;

typedef enum {
    SKIPLIST_OK = 0,
    SKIPLIST_ERROR_INVALID_PARAMS = -1,
    SKIPLIST_ERROR_CREATE = -2,
    SKIPLIST_ERROR_INSERT = -3,
    SKIPLIST_ERROR_DELETE = -4,
    SKIPLIST_ERROR_GET = -5,
    SKIPLIST_ERROR_ITERATOR = -6,
    SKIPLIST_ERROR_MEMORY = -7,
    SKIPLIST_ERROR_NODE_NOT_FOUND = -8,
    SKIPLIST_ERROR_COMPARATOR_NULL = -9
} skiplist_error_t;

typedef struct {
    int max_level;              // 最大层级
    bool allow_duplicates;      // 允许重复键
    bool use_malloc;            // 使用malloc分配内存
    bool enable_statistics;     // 启用统计
    size_t initial_capacity;    // 初始容量
    float probability;          // 层级生成概率
} skiplist_config_t;

typedef struct {
    skiplist_error_t last_error;     // 最后一次错误
    size_t operations;               // 操作计数
    size_t insertions;               // 插入计数
    size_t deletions;                // 删除计数
    size_t lookups;                  // 查找计数
    size_t collisions;               // 碰撞计数
    bool is_initialized;             // 是否初始化
    uint32_t random_seed;            // 随机种子
} skiplist_state_t;

/**
 * @brief 跳表迭代器结构体
 */
typedef struct {
    skiplist_t *sl;
    skiplist_node_t *current;
    bool is_valid;
} skiplist_iterator_t;

/**
 * @brief 创建跳表
 * @param compar 比较函数
 * @return 跳表指针，失败返回NULL
 */
skiplist_t* skiplist_create(int (*compar)(const void *, const void *));

/**
 * @brief 增强版跳表创建
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 跳表指针，失败返回NULL
 */
skiplist_t* skiplist_create_ex(int (*compar)(const void *, const void *), const skiplist_config_t *config, skiplist_state_t *state);

/**
 * @brief 释放跳表
 * @param sl 跳表指针
 */
void skiplist_free(skiplist_t *sl);

/**
 * @brief 增强版跳表释放
 * @param sl 跳表指针
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_free_ex(skiplist_t *sl, skiplist_state_t *state);

/**
 * @brief 插入键值对
 * @param sl 跳表指针
 * @param key 键
 * @param value 值
 */
void skiplist_insert(skiplist_t *sl, void *key, void *value);

/**
 * @brief 增强版插入键值对
 * @param sl 跳表指针
 * @param key 键
 * @param value 值
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_insert_ex(skiplist_t *sl, void *key, void *value, skiplist_state_t *state);

/**
 * @brief 获取值
 * @param sl 跳表指针
 * @param key 键
 * @return 值，失败返回NULL
 */
void* skiplist_get(skiplist_t *sl, const void *key);

/**
 * @brief 增强版获取值
 * @param sl 跳表指针
 * @param key 键
 * @param value 输出值
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_get_ex(skiplist_t *sl, const void *key, void **value, skiplist_state_t *state);

/**
 * @brief 删除键值对
 * @param sl 跳表指针
 * @param key 键
 */
void skiplist_delete(skiplist_t *sl, const void *key);

/**
 * @brief 增强版删除键值对
 * @param sl 跳表指针
 * @param key 键
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_delete_ex(skiplist_t *sl, const void *key, skiplist_state_t *state);

/**
 * @brief 检查键是否存在
 * @param sl 跳表指针
 * @param key 键
 * @param exists 输出结果
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_contains(skiplist_t *sl, const void *key, bool *exists, skiplist_state_t *state);

/**
 * @brief 获取跳表大小
 * @param sl 跳表指针
 * @return 跳表大小
 */
size_t skiplist_size(skiplist_t *sl);

/**
 * @brief 检查跳表是否为空
 * @param sl 跳表指针
 * @return 是否为空
 */
bool skiplist_is_empty(skiplist_t *sl);

/**
 * @brief 清空跳表
 * @param sl 跳表指针
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_clear(skiplist_t *sl, skiplist_state_t *state);

/**
 * @brief 创建迭代器
 * @param sl 跳表指针
 * @param iterator 输出迭代器
 * @return 错误码
 */
skiplist_error_t skiplist_iterator_create(skiplist_t *sl, skiplist_iterator_t *iterator);

/**
 * @brief 获取下一个元素
 * @param iterator 迭代器
 * @param key 输出键
 * @param value 输出值
 * @return 是否有下一个元素
 */
bool skiplist_iterator_next(skiplist_iterator_t *iterator, void **key, void **value);

/**
 * @brief 重置迭代器
 * @param iterator 迭代器
 * @return 错误码
 */
skiplist_error_t skiplist_iterator_reset(skiplist_iterator_t *iterator);

/**
 * @brief 释放迭代器
 * @param iterator 迭代器
 */
void skiplist_iterator_free(skiplist_iterator_t *iterator);

/**
 * @brief 批量插入
 * @param sl 跳表指针
 * @param keys 键数组
 * @param values 值数组
 * @param count 数量
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_batch_insert(skiplist_t *sl, void **keys, void **values, size_t count, skiplist_state_t *state);

/**
 * @brief 批量删除
 * @param sl 跳表指针
 * @param keys 键数组
 * @param count 数量
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_batch_delete(skiplist_t *sl, void **keys, size_t count, skiplist_state_t *state);

/**
 * @brief 获取第一个元素
 * @param sl 跳表指针
 * @param key 输出键
 * @param value 输出值
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_first(skiplist_t *sl, void **key, void **value, skiplist_state_t *state);

/**
 * @brief 获取最后一个元素
 * @param sl 跳表指针
 * @param key 输出键
 * @param value 输出值
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_last(skiplist_t *sl, void **key, void **value, skiplist_state_t *state);

/**
 * @brief 获取小于等于指定键的最大元素
 * @param sl 跳表指针
 * @param key 键
 * @param result_key 输出键
 * @param result_value 输出值
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_find_less_or_equal(skiplist_t *sl, const void *key, void **result_key, void **result_value, skiplist_state_t *state);

/**
 * @brief 获取大于等于指定键的最小元素
 * @param sl 跳表指针
 * @param key 键
 * @param result_key 输出键
 * @param result_value 输出值
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_find_greater_or_equal(skiplist_t *sl, const void *key, void **result_key, void **result_value, skiplist_state_t *state);

/**
 * @brief 获取统计信息
 * @param sl 跳表指针
 * @param state 状态输出
 * @return 错误码
 */
skiplist_error_t skiplist_get_statistics(skiplist_t *sl, skiplist_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* skiplist_strerror(const skiplist_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void skiplist_config_init(skiplist_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void skiplist_state_init(skiplist_state_t *state);

#endif // C_UTILS_SKIPLIST_H
