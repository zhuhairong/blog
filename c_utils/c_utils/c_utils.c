/**
 * @file c_utils.c
 * @brief c_utils 公共头文件的实现
 *
 * 提供版本信息查询和错误码字符串转换。
 */

#include "c_utils.h"

/* =========================================================================
 * 字符串化辅助宏
 * ========================================================================= */

#define CU_STR_IMPL(x)  #x
#define CU_STR(x)       CU_STR_IMPL(x)

/* =========================================================================
 * 版本信息
 * ========================================================================= */

const char* cu_version_string(void) {
    return CU_STR(C_UTILS_VERSION_MAJOR) "."
           CU_STR(C_UTILS_VERSION_MINOR) "."
           CU_STR(C_UTILS_VERSION_PATCH);
}

/* =========================================================================
 * 错误码字符串
 * ========================================================================= */

const char* cu_strerror(cu_error_t error) {
    switch (error) {
        case CU_OK:            return "Success";
        case CU_ERROR_NULL:    return "NULL pointer parameter";
        case CU_ERROR_MEMORY:  return "Memory allocation failed";
        case CU_ERROR_RANGE:   return "Index or value out of range";
        case CU_ERROR_IO:      return "I/O error";
        case CU_ERROR_FORMAT:  return "Invalid format";
        case CU_ERROR_STATE:   return "Invalid state";
        case CU_ERROR_TIMEOUT: return "Operation timed out";
        case CU_ERROR_UNKNOWN: return "Unknown error";
        default:               return "Unrecognized error";
    }
}
