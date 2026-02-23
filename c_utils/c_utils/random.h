#ifndef C_UTILS_RANDOM_H
#define C_UTILS_RANDOM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief 随机数生成器错误码
 */
typedef enum {
    RANDOM_OK = 0,                  /**< 成功 */
    RANDOM_ERROR_NULL_PTR,          /**< 空指针错误 */
    RANDOM_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RANDOM_ERROR_RANGE_ERROR,       /**< 范围错误 */
    RANDOM_ERROR_STATE_ERROR,       /**< 状态错误 */
    RANDOM_ERROR_MAX                /**< 最大错误码 */
} random_error_t;

/**
 * @brief 随机数生成器类型
 */
typedef enum {
    RANDOM_TYPE_XORSHIFT = 0,       /**< XORShift 算法 */
    RANDOM_TYPE_MERSENNE_TWISTER,   /**< Mersenne Twister 算法 */
    RANDOM_TYPE_LCG,                /**< 线性同余算法 */
    RANDOM_TYPE_SYSTEM,             /**< 系统随机数 */
    RANDOM_TYPE_MAX                 /**< 最大算法类型 */
} random_type_t;

/**
 * @brief 随机数生成器配置
 */
typedef struct {
    random_type_t type;             /**< 生成器类型 */
    uint64_t seed;                  /**< 种子值 */
    bool use_crypto_seed;           /**< 是否使用加密安全的种子 */
    size_t buffer_size;             /**< 缓冲区大小 (0 表示无缓冲) */
} random_config_t;

/**
 * @brief 随机数生成器上下文
 */
typedef struct {
    random_config_t config;         /**< 配置 */
    union {
        struct {
            uint64_t x;             /**< XORShift 状态 */
            uint64_t y;             /**< XORShift 状态 */
            uint64_t z;             /**< XORShift 状态 */
            uint64_t w;             /**< XORShift 状态 */
        } xorshift;
        struct {
            uint32_t mt[624];        /**< Mersenne Twister 状态 */
            int index;              /**< 当前索引 */
        } mt;
        struct {
            uint64_t state;          /**< LCG 状态 */
            uint64_t a;              /**< LCG 乘数 */
            uint64_t c;              /**< LCG 增量 */
            uint64_t m;              /**< LCG 模数 */
        } lcg;
    } state;                        /**< 生成器状态 */
    uint8_t *buffer;                /**< 随机数缓冲区 */
    size_t buffer_pos;              /**< 缓冲区位置 */
    size_t buffer_size;             /**< 缓冲区大小 */
    bool initialized;               /**< 是否已初始化 */
} random_ctx_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
random_config_t random_default_config(void);

/**
 * @brief 设置全局随机数种子
 * @param seed 种子值
 */
void random_seed(uint64_t seed);

/**
 * @brief 使用系统熵设置全局种子
 * @return 是否成功
 */
bool random_seed_with_entropy(void);

/**
 * @brief 获取 64 位无符号随机整数
 * @return 随机整数
 */
uint64_t random_u64(void);

/**
 * @brief 获取 32 位无符号随机整数
 * @return 随机整数
 */
uint32_t random_u32(void);

/**
 * @brief 获取 16 位无符号随机整数
 * @return 随机整数
 */
uint16_t random_u16(void);

/**
 * @brief 获取 8 位无符号随机整数
 * @return 随机整数
 */
uint8_t random_u8(void);

/**
 * @brief 获取有符号 64 位随机整数
 * @return 随机整数
 */
int64_t random_i64(void);

/**
 * @brief 获取有符号 32 位随机整数
 * @return 随机整数
 */
int32_t random_i32(void);

/**
 * @brief 获取 [min, max] 范围内的随机整数
 * @param min 最小值
 * @param max 最大值
 * @return 随机整数
 */
int64_t random_range(int64_t min, int64_t max);

/**
 * @brief 获取 [min, max] 范围内的随机整数（带错误处理）
 * @param min 最小值
 * @param max 最大值
 * @param error 错误码输出
 * @return 随机整数
 */
int64_t random_range_ex(int64_t min, int64_t max, random_error_t *error);

/**
 * @brief 获取 [0.0, 1.0] 范围内的随机浮点数
 * @return 随机浮点数
 */
double random_double(void);

/**
 * @brief 获取 [min, max] 范围内的随机浮点数
 * @param min 最小值
 * @param max 最大值
 * @return 随机浮点数
 */
double random_double_range(double min, double max);

/**
 * @brief 获取 [min, max] 范围内的随机浮点数（带错误处理）
 * @param min 最小值
 * @param max 最大值
 * @param error 错误码输出
 * @return 随机浮点数
 */
double random_double_range_ex(double min, double max, random_error_t *error);

/**
 * @brief 获取随机布尔值
 * @param probability 为 true 的概率 (0.0-1.0)
 * @return 随机布尔值
 */
bool random_bool(double probability);

/**
 * @brief 生成随机字节
 * @param buffer 缓冲区
 * @param size 缓冲区大小
 * @return 是否成功
 */
bool random_bytes(uint8_t *buffer, size_t size);

/**
 * @brief 生成随机字节（带错误处理）
 * @param buffer 缓冲区
 * @param size 缓冲区大小
 * @param error 错误码输出
 * @return 是否成功
 */
bool random_bytes_ex(uint8_t *buffer, size_t size, random_error_t *error);

/**
 * @brief 初始化随机数生成器上下文
 * @param ctx 上下文
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool random_init(random_ctx_t *ctx, const random_config_t *config, random_error_t *error);

/**
 * @brief 从上下文获取 64 位无符号随机整数
 * @param ctx 上下文
 * @return 随机整数
 */
uint64_t random_u64_ctx(random_ctx_t *ctx);

/**
 * @brief 从上下文获取 [min, max] 范围内的随机整数
 * @param ctx 上下文
 * @param min 最小值
 * @param max 最大值
 * @param error 错误码输出
 * @return 随机整数
 */
int64_t random_range_ctx(random_ctx_t *ctx, int64_t min, int64_t max, random_error_t *error);

/**
 * @brief 从上下文获取 [0.0, 1.0] 范围内的随机浮点数
 * @param ctx 上下文
 * @return 随机浮点数
 */
double random_double_ctx(random_ctx_t *ctx);

/**
 * @brief 从上下文生成随机字节
 * @param ctx 上下文
 * @param buffer 缓冲区
 * @param size 缓冲区大小
 * @param error 错误码输出
 * @return 是否成功
 */
bool random_bytes_ctx(random_ctx_t *ctx, uint8_t *buffer, size_t size, random_error_t *error);

/**
 * @brief 重置随机数生成器上下文
 * @param ctx 上下文
 * @param seed 种子值
 * @param error 错误码输出
 * @return 是否成功
 */
bool random_reset(random_ctx_t *ctx, uint64_t seed, random_error_t *error);

/**
 * @brief 销毁随机数生成器上下文
 * @param ctx 上下文
 */
void random_destroy(random_ctx_t *ctx);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* random_error_string(random_error_t error);

#endif // C_UTILS_RANDOM_H
