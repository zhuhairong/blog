#ifndef C_UTILS_STATS_H
#define C_UTILS_STATS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    double min;
    double max;
    double mean;
    double variance;
    double stddev;
} stats_t;

typedef enum {
    STATS_OK = 0,
    STATS_ERROR_INVALID_PARAMS = -1,
    STATS_ERROR_COMPUTE = -2,
    STATS_ERROR_MEMORY = -3,
    STATS_ERROR_SIZE_ZERO = -4,
    STATS_ERROR_DATA_NULL = -5,
    STATS_ERROR_FILE_OPEN = -6,
    STATS_ERROR_FILE_READ = -7,
    STATS_ERROR_FILE_WRITE = -8,
    STATS_ERROR_DIVISION_BY_ZERO = -9,
    STATS_ERROR_NAN_VALUE = -10,
    STATS_ERROR_INFINITE_VALUE = -11,
    STATS_ERROR_OUT_OF_RANGE = -12
} stats_error_t;

typedef struct {
    bool enable_outlier_detection;   // 启用异常值检测
    bool enable_skewness_kurtosis;   // 启用偏度和峰度计算
    bool enable_percentiles;         // 启用百分位数计算
    bool enable_statistics;          // 启用统计
    bool ignore_nan;                 // 忽略NaN值
    bool ignore_infinite;            // 忽略无限值
    double outlier_threshold;        // 异常值阈值
    size_t max_data_size;            // 最大数据大小
    size_t percentile_count;         // 百分位数数量
} stats_config_t;

typedef struct {
    stats_error_t last_error;         // 最后一次错误
    size_t computations;              // 计算次数
    size_t data_points_processed;     // 处理的数据点数
    bool is_initialized;              // 是否初始化
    size_t average_data_size;         // 平均数据大小
    double processing_time;           // 处理时间（毫秒）
} stats_state_t;

/**
 * @brief 扩展统计结构体
 */
typedef struct {
    double min;
    double max;
    double mean;
    double variance;
    double stddev;
    double median;
    double skewness;
    double kurtosis;
    double sum;
    double sum_squares;
    size_t count;
    size_t valid_count;
    double q1;  // 第一四分位数
    double q3;  // 第三四分位数
    double iqr; // 四分位距
} stats_extended_t;

/**
 * @brief 计算基本统计信息
 * @param data 数据数组
 * @param n 数据大小
 * @return 统计结果
 */
stats_t stats_compute(const double *data, size_t n);

/**
 * @brief 增强版统计计算
 * @param data 数据数组
 * @param n 数据大小
 * @param config 配置选项
 * @param result 输出结果
 * @param extended_result 扩展结果（可选）
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_compute_ex(const double *data, size_t n,
                              const stats_config_t *config,
                              stats_t *result,
                              stats_extended_t *extended_result,
                              stats_state_t *state);

/**
 * @brief 计算中位数
 * @param data 数据数组
 * @param n 数据大小
 * @param config 配置选项
 * @param median 输出中位数
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_median(const double *data, size_t n,
                          const stats_config_t *config,
                          double *median,
                          stats_state_t *state);

/**
 * @brief 计算百分位数
 * @param data 数据数组
 * @param n 数据大小
 * @param percentiles 百分位数数组
 * @param count 百分位数数量
 * @param config 配置选项
 * @param results 输出结果
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_percentiles(const double *data, size_t n,
                               const double *percentiles, size_t count,
                               const stats_config_t *config,
                               double *results,
                               stats_state_t *state);

/**
 * @brief 计算偏度和峰度
 * @param data 数据数组
 * @param n 数据大小
 * @param config 配置选项
 * @param skewness 输出偏度
 * @param kurtosis 输出峰度
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_skewness_kurtosis(const double *data, size_t n,
                                     const stats_config_t *config,
                                     double *skewness, double *kurtosis,
                                     stats_state_t *state);

/**
 * @brief 检测异常值
 * @param data 数据数组
 * @param n 数据大小
 * @param config 配置选项
 * @param outliers 输出异常值索引
 * @param outlier_count 输出异常值数量
 * @param max_outliers 最大异常值数量
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_detect_outliers(const double *data, size_t n,
                                   const stats_config_t *config,
                                   size_t *outliers, size_t *outlier_count,
                                   size_t max_outliers,
                                   stats_state_t *state);

/**
 * @brief 计算相关性
 * @param data1 第一个数据数组
 * @param data2 第二个数据数组
 * @param n 数据大小
 * @param config 配置选项
 * @param correlation 输出相关系数
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_correlation(const double *data1, const double *data2, size_t n,
                               const stats_config_t *config,
                               double *correlation,
                               stats_state_t *state);

/**
 * @brief 计算线性回归
 * @param x x数据数组
 * @param y y数据数组
 * @param n 数据大小
 * @param config 配置选项
 * @param slope 输出斜率
 * @param intercept 输出截距
 * @param r_squared 输出R²
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_linear_regression(const double *x, const double *y, size_t n,
                                     const stats_config_t *config,
                                     double *slope, double *intercept, double *r_squared,
                                     stats_state_t *state);

/**
 * @brief 计算移动平均
 * @param data 数据数组
 * @param n 数据大小
 * @param window_size 窗口大小
 * @param config 配置选项
 * @param result 输出结果
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_moving_average(const double *data, size_t n, size_t window_size,
                                  const stats_config_t *config,
                                  double *result,
                                  stats_state_t *state);

/**
 * @brief 从文件读取数据并计算统计信息
 * @param filename 文件名
 * @param config 配置选项
 * @param result 输出结果
 * @param extended_result 扩展结果（可选）
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_compute_from_file(const char *filename,
                                     const stats_config_t *config,
                                     stats_t *result,
                                     stats_extended_t *extended_result,
                                     stats_state_t *state);

/**
 * @brief 保存统计结果到文件
 * @param filename 文件名
 * @param result 统计结果
 * @param extended_result 扩展结果（可选）
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_save_to_file(const char *filename,
                                const stats_t *result,
                                const stats_extended_t *extended_result,
                                stats_state_t *state);

/**
 * @brief 验证数据
 * @param data 数据数组
 * @param n 数据大小
 * @param config 配置选项
 * @param valid 输出验证结果
 * @param invalid_count 输出无效数据数量
 * @param state 状态输出
 * @return 错误码
 */
stats_error_t stats_validate_data(const double *data, size_t n,
                                 const stats_config_t *config,
                                 bool *valid, size_t *invalid_count,
                                 stats_state_t *state);

/**
 * @brief 获取统计信息
 * @param state 状态结构
 * @return 错误码
 */
stats_error_t stats_get_statistics(stats_state_t *state);

/**
 * @brief 重置统计状态
 * @param state 状态结构
 */
void stats_reset_state(stats_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* stats_strerror(const stats_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void stats_config_init(stats_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void stats_state_init(stats_state_t *state);

#endif // C_UTILS_STATS_H
