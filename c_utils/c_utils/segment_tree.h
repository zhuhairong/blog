#ifndef C_UTILS_SEGMENT_TREE_H
#define C_UTILS_SEGMENT_TREE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief 线段树错误码
 */
typedef enum {
    SEGMENT_TREE_OK = 0,                  /**< 成功 */
    SEGMENT_TREE_ERROR_NULL_PTR,          /**< 空指针错误 */
    SEGMENT_TREE_ERROR_INVALID_ARGS,      /**< 无效参数 */
    SEGMENT_TREE_ERROR_SIZE_TOO_SMALL,    /**< 大小过小 */
    SEGMENT_TREE_ERROR_INDEX_OUT_OF_BOUNDS, /**< 索引越界 */
    SEGMENT_TREE_ERROR_RANGE_INVALID,     /**< 范围无效 */
    SEGMENT_TREE_ERROR_MEMORY_ALLOC,      /**< 内存分配失败 */
    SEGMENT_TREE_ERROR_NOT_INITIALIZED,    /**< 未初始化 */
    SEGMENT_TREE_ERROR_MAX                /**< 最大错误码 */
} segment_tree_error_t;

/**
 * @brief 线段树操作类型
 */
typedef enum {
    SEGMENT_TREE_OP_MIN = 0,              /**< 最小值查询 */
    SEGMENT_TREE_OP_MAX_VAL,              /**< 最大值查询 */
    SEGMENT_TREE_OP_SUM,                  /**< 和查询 */
    SEGMENT_TREE_OP_RANGE_UPDATE,         /**< 范围更新 */
    SEGMENT_TREE_OP_COUNT                 /**< 操作类型数量 */
} segment_tree_op_t;

/**
 * @brief 线段树配置
 */
typedef struct {
    segment_tree_op_t operation;         /**< 操作类型 */
    bool use_optimized;                 /**< 是否使用优化模式 */
    bool check_bounds;                  /**< 是否检查边界 */
    bool use_lazy_propagation;          /**< 是否使用延迟传播 */
    size_t max_size;                    /**< 最大大小 */
    int default_value;                  /**< 默认值 */
} segment_tree_config_t;

/**
 * @brief 线段树状态
 */
typedef struct {
    size_t size;                        /**< 原始数组大小 */
    size_t tree_size;                   /**< 线段树大小 */
    segment_tree_op_t operation;         /**< 当前操作类型 */
    bool initialized;                   /**< 是否已初始化 */
    bool use_lazy;                      /**< 是否使用延迟传播 */
} segment_tree_state_t;

/**
 * @brief 线段树结构
 */
typedef struct {
    int *tree;                           /**< 线段树数组 */
    int *lazy;                           /**< 延迟传播数组 */
    size_t n;                            /**< 原始数组大小 */
    size_t tree_size;                     /**< 线段树大小 */
    segment_tree_config_t config;         /**< 配置 */
    bool initialized;                    /**< 是否已初始化 */
} segment_tree_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
segment_tree_config_t segment_tree_default_config(void);

/**
 * @brief 初始化线段树
 * @param st 线段树指针
 * @param arr 输入数组
 * @param n 数组大小
 */
void segment_tree_init(segment_tree_t *st, int *arr, size_t n);

/**
 * @brief 使用自定义配置初始化线段树
 * @param st 线段树指针
 * @param arr 输入数组
 * @param n 数组大小
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool segment_tree_init_ex(segment_tree_t *st, int *arr, size_t n, const segment_tree_config_t *config, segment_tree_error_t *error);

/**
 * @brief 更新线段树中的单个元素
 * @param st 线段树指针
 * @param idx 元素索引
 * @param val 新值
 */
void segment_tree_update(segment_tree_t *st, size_t idx, int val);

/**
 * @brief 更新线段树中的单个元素（带错误处理）
 * @param st 线段树指针
 * @param idx 元素索引
 * @param val 新值
 * @param error 错误码输出
 * @return 是否成功
 */
bool segment_tree_update_ex(segment_tree_t *st, size_t idx, int val, segment_tree_error_t *error);

/**
 * @brief 查询线段树中的区间
 * @param st 线段树指针
 * @param left 左边界
 * @param right 右边界
 * @return 查询结果
 */
int segment_tree_query(segment_tree_t *st, size_t left, size_t right);

/**
 * @brief 查询线段树中的区间（带错误处理）
 * @param st 线段树指针
 * @param left 左边界
 * @param right 右边界
 * @param error 错误码输出
 * @return 查询结果，失败返回默认值
 */
int segment_tree_query_ex(segment_tree_t *st, size_t left, size_t right, segment_tree_error_t *error);

/**
 * @brief 范围更新线段树
 * @param st 线段树指针
 * @param left 左边界
 * @param right 右边界
 * @param val 更新值
 * @param error 错误码输出
 * @return 是否成功
 */
bool segment_tree_range_update(segment_tree_t *st, size_t left, size_t right, int val, segment_tree_error_t *error);

/**
 * @brief 释放线段树
 * @param st 线段树指针
 */
void segment_tree_free(segment_tree_t *st);

/**
 * @brief 释放线段树（带错误处理）
 * @param st 线段树指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool segment_tree_free_ex(segment_tree_t *st, segment_tree_error_t *error);

/**
 * @brief 获取线段树状态
 * @param st 线段树指针
 * @param state 状态输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool segment_tree_get_state(const segment_tree_t *st, segment_tree_state_t *state, segment_tree_error_t *error);

/**
 * @brief 重置线段树
 * @param st 线段树指针
 * @param arr 新的输入数组
 * @param n 数组大小
 * @param error 错误码输出
 * @return 是否成功
 */
bool segment_tree_reset(segment_tree_t *st, int *arr, size_t n, segment_tree_error_t *error);

/**
 * @brief 检查线段树是否已初始化
 * @param st 线段树指针
 * @return 是否已初始化
 */
bool segment_tree_is_initialized(const segment_tree_t *st);

/**
 * @brief 获取线段树大小
 * @param st 线段树指针
 * @return 原始数组大小
 */
size_t segment_tree_get_size(const segment_tree_t *st);

/**
 * @brief 获取线段树的树大小
 * @param st 线段树指针
 * @return 线段树大小
 */
size_t segment_tree_get_tree_size(const segment_tree_t *st);

/**
 * @brief 验证线段树的有效性
 * @param st 线段树指针
 * @param error 错误码输出
 * @return 是否有效
 */
bool segment_tree_validate(const segment_tree_t *st, segment_tree_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* segment_tree_error_string(segment_tree_error_t error);

#endif // C_UTILS_SEGMENT_TREE_H
