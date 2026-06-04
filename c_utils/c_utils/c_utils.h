/**
 * @file c_utils.h
 * @brief c_utils 公共头文件 — 通用类型、错误码、编译器注解和版本信息
 *
 * 此头文件为 c_utils 库提供统一的入口，包含：
 *   - 库版本信息
 *   - 通用错误码约定 (cu_error_t)
 *   - 编译器辅助宏 (CU_UNUSED, CU_DEPRECATED, CU_NORETURN 等)
 *   - 实用工具宏 (CU_ARRAY_SIZE, CU_MIN, CU_MAX)
 *
 * 注意：此文件不修改现有模块的 API，保持向后兼容。
 *       仅供新代码和外部用户使用统一接口。
 */

#ifndef C_UTILS_H
#define C_UTILS_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * 库版本信息
 * ========================================================================= */

#define C_UTILS_VERSION_MAJOR 1
#define C_UTILS_VERSION_MINOR 0
#define C_UTILS_VERSION_PATCH 0

/** 库版本字符串 "major.minor.patch" */
const char* cu_version_string(void);

/** 库版本数值（便于比较：MAJOR*10000 + MINOR*100 + PATCH） */
#define C_UTILS_VERSION_NUM \
    (C_UTILS_VERSION_MAJOR * 10000 + C_UTILS_VERSION_MINOR * 100 + C_UTILS_VERSION_PATCH)

/* =========================================================================
 * 通用错误码
 * ========================================================================= */

/**
 * cu_error_t — 库内通用的操作结果码
 *
 * 成功返回 CU_OK (0)，其他为错误。各模块可扩展自己的错误码范围。
 */
typedef enum {
    CU_OK            =  0,   /**< 操作成功 */
    CU_ERROR_NULL    = -1,   /**< NULL 指针参数 */
    CU_ERROR_MEMORY  = -2,   /**< 内存分配失败 */
    CU_ERROR_RANGE   = -3,   /**< 索引或值超出范围 */
    CU_ERROR_IO      = -4,   /**< I/O 错误 */
    CU_ERROR_FORMAT  = -5,   /**< 格式无效 */
    CU_ERROR_STATE   = -6,   /**< 状态机错误 */
    CU_ERROR_TIMEOUT = -7,   /**< 操作超时 */
    CU_ERROR_UNKNOWN = -99   /**< 未知错误 */
} cu_error_t;

/** 将错误码转为可读字符串 */
const char* cu_strerror(cu_error_t error);

/* =========================================================================
 * 编译器辅助宏
 * ========================================================================= */

/** 标记未使用的变量/参数，消除编译器警告 */
#if defined(__GNUC__) || defined(__clang__)
    #define CU_UNUSED(x)       (void)(x)
#else
    #define CU_UNUSED(x)       /* x */
#endif

/** 标记函数为废弃 */
#if defined(__GNUC__) || defined(__clang__)
    #define CU_DEPRECATED(msg)  __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
    #define CU_DEPRECATED(msg)  __declspec(deprecated(msg))
#else
    #define CU_DEPRECATED(msg)
#endif

/** 标记函数不返回（如 exit/abort） */
#if defined(__GNUC__) || defined(__clang__)
    #define CU_NORETURN         __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define CU_NORETURN         __declspec(noreturn)
#else
    #define CU_NORETURN
#endif

/** 标记函数为 printf 风格（启用格式字符串检查） */
#if defined(__GNUC__) || defined(__clang__)
    #define CU_PRINTF_LIKE(fmt_idx, arg_idx)  __attribute__((format(printf, fmt_idx, arg_idx)))
#else
    #define CU_PRINTF_LIKE(fmt_idx, arg_idx)
#endif

/** 标记可能未使用的函数/变量（如静态内联） */
#if defined(__GNUC__) || defined(__clang__)
    #define CU_MAYBE_UNUSED     __attribute__((unused))
#else
    #define CU_MAYBE_UNUSED
#endif

/** 分支预测提示: likely */
#if defined(__GNUC__) || defined(__clang__)
    #define CU_LIKELY(x)        __builtin_expect(!!(x), 1)
    #define CU_UNLIKELY(x)      __builtin_expect(!!(x), 0)
#else
    #define CU_LIKELY(x)        (x)
    #define CU_UNLIKELY(x)      (x)
#endif

/* =========================================================================
 * 实用工具宏
 * ========================================================================= */

/** 静态数组元素个数 */
#define CU_ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

/** 取两值中较小者 */
#define CU_MIN(a, b)        (((a) < (b)) ? (a) : (b))

/** 取两值中较大者 */
#define CU_MAX(a, b)        (((a) > (b)) ? (a) : (b))

/** 将值钳制在 [lo, hi] 范围内 */
#define CU_CLAMP(x, lo, hi) (CU_MIN(CU_MAX((x), (lo)), (hi)))

#ifdef __cplusplus
}
#endif

#endif /* C_UTILS_H */
