#ifndef C_UTILS_SPARSE_TABLE_H
#define C_UTILS_SPARSE_TABLE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int **st;
    size_t n;
    size_t max_log;
} sparse_table_t;

typedef enum {
    SPARSE_TABLE_OK = 0,
    SPARSE_TABLE_ERROR_INVALID_PARAMS = -1,
    SPARSE_TABLE_ERROR_INIT = -2,
    SPARSE_TABLE_ERROR_QUERY = -3,
    SPARSE_TABLE_ERROR_FREE = -4,
    SPARSE_TABLE_ERROR_MEMORY = -5,
    SPARSE_TABLE_ERROR_SIZE_ZERO = -6,
    SPARSE_TABLE_ERROR_INDEX_OUT_OF_BOUNDS = -7,
    SPARSE_TABLE_ERROR_COMPARATOR_NULL = -8,
    SPARSE_TABLE_ERROR_NOT_INITIALIZED = -9,
    SPARSE_TABLE_ERROR_FILE_OPEN = -10,
    SPARSE_TABLE_ERROR_FILE_READ = -11,
    SPARSE_TABLE_ERROR_FILE_WRITE = -12
} sparse_table_error_t;

typedef enum {
    SPARSE_TABLE_OPERATION_MIN = 0,  // 最小值
    SPARSE_TABLE_OPERATION_MAX = 1,  // 最大值
    SPARSE_TABLE_OPERATION_SUM = 2,  // 和
    SPARSE_TABLE_OPERATION_GCD = 3,  // 最大公约数
    SPARSE_TABLE_OPERATION_LCM = 4,  // 最小公倍数
    SPARSE_TABLE_OPERATION_CUSTOM = 5  // 自定义操作
} sparse_table_operation_t;

typedef struct {
    sparse_table_operation_t operation;  // 操作类型
    bool use_malloc;               // 使用malloc分配内存
    bool enable_statistics;        // 启用统计
    bool enable_optimizations;     // 启用优化
    size_t max_size;               // 最大大小
    size_t initial_size;           // 初始大小
    bool use_exponential_search;   // 使用指数搜索
} sparse_table_config_t;

typedef struct {
    sparse_table_error_t last_error;     // 最后一次错误
    size_t initializations;              // 初始化计数
    size_t queries;                      // 查询计数
    bool is_initialized;                 // 是否初始化
    size_t average_query_time;           // 平均查询时间
    size_t memory_used;                  // 内存使用
    size_t operations_performed;         // 执行的操作数
} sparse_table_state_t;

/**
 * @brief 自定义操作函数类型
 */
typedef int (*sparse_table_custom_operation_t)(int a, int b);

/**
 * @brief 初始化稀疏表
 * @param st 稀疏表结构体
 * @param arr 输入数组
 * @param n 数组大小
 */
void sparse_table_init(sparse_table_t *st, int *arr, size_t n);

/**
 * @brief 增强版稀疏表初始化
 * @param st 稀疏表结构体
 * @param arr 输入数组
 * @param n 数组大小
 * @param config 配置选项
 * @param custom_op 自定义操作函数
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_init_ex(sparse_table_t *st, int *arr, size_t n,
                                         const sparse_table_config_t *config,
                                         sparse_table_custom_operation_t custom_op,
                                         sparse_table_state_t *state);

/**
 * @brief 查询区间
 * @param st 稀疏表结构体
 * @param l 左边界
 * @param r 右边界
 * @return 查询结果
 */
int sparse_table_query(sparse_table_t *st, size_t l, size_t r);

/**
 * @brief 增强版区间查询
 * @param st 稀疏表结构体
 * @param l 左边界
 * @param r 右边界
 * @param result 输出结果
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_query_ex(sparse_table_t *st, size_t l, size_t r,
                                         int *result,
                                         sparse_table_state_t *state);

/**
 * @brief 释放稀疏表
 * @param st 稀疏表结构体
 */
void sparse_table_free(sparse_table_t *st);

/**
 * @brief 增强版稀疏表释放
 * @param st 稀疏表结构体
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_free_ex(sparse_table_t *st,
                                         sparse_table_state_t *state);

/**
 * @brief 批量查询
 * @param st 稀疏表结构体
 * @param queries 查询数组
 * @param count 查询数量
 * @param results 结果数组
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_batch_query(sparse_table_t *st,
                                             const size_t (*queries)[2],
                                             size_t count,
                                             int *results,
                                             sparse_table_state_t *state);

/**
 * @brief 动态更新稀疏表
 * @param st 稀疏表结构体
 * @param index 索引
 * @param value 新值
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_update(sparse_table_t *st,
                                        size_t index,
                                        int value,
                                        sparse_table_state_t *state);

/**
 * @brief 构建稀疏表从文件
 * @param st 稀疏表结构体
 * @param filename 文件名
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_build_from_file(sparse_table_t *st,
                                                 const char *filename,
                                                 const sparse_table_config_t *config,
                                                 sparse_table_state_t *state);

/**
 * @brief 保存稀疏表到文件
 * @param st 稀疏表结构体
 * @param filename 文件名
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_save_to_file(sparse_table_t *st,
                                              const char *filename,
                                              sparse_table_state_t *state);

/**
 * @brief 验证稀疏表
 * @param st 稀疏表结构体
 * @param original_arr 原始数组
 * @param n 数组大小
 * @param valid 输出验证结果
 * @param state 状态输出
 * @return 错误码
 */
sparse_table_error_t sparse_table_validate(sparse_table_t *st,
                                          const int *original_arr,
                                          size_t n,
                                          bool *valid,
                                          sparse_table_state_t *state);

/**
 * @brief 获取稀疏表信息
 * @param st 稀疏表结构体
 * @param size 输出大小
 * @param max_log 输出最大对数
 * @param memory_used 输出内存使用
 * @return 错误码
 */
sparse_table_error_t sparse_table_get_info(sparse_table_t *st,
                                          size_t *size,
                                          size_t *max_log,
                                          size_t *memory_used);

/**
 * @brief 获取稀疏表统计信息
 * @param state 状态结构
 * @return 错误码
 */
sparse_table_error_t sparse_table_get_statistics(sparse_table_state_t *state);

/**
 * @brief 重置稀疏表状态
 * @param state 状态结构
 */
void sparse_table_reset_state(sparse_table_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* sparse_table_strerror(const sparse_table_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void sparse_table_config_init(sparse_table_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void sparse_table_state_init(sparse_table_state_t *state);

#endif // C_UTILS_SPARSE_TABLE_H
