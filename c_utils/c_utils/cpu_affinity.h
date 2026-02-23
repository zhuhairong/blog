#ifndef C_UTILS_CPU_AFFINITY_H
#define C_UTILS_CPU_AFFINITY_H

#include <stdbool.h>
#include <stdint.h>

// CPU 亲和性错误码
typedef enum {
    CPU_AFFINITY_OK = 0,
    CPU_AFFINITY_ERROR_INVALID_CPU,
    CPU_AFFINITY_ERROR_INVALID_THREAD,
    CPU_AFFINITY_ERROR_OPERATION_FAILED,
    CPU_AFFINITY_ERROR_PLATFORM_UNSUPPORTED,
    CPU_AFFINITY_ERROR_MEMORY_ALLOC
} cpu_affinity_error_t;

// CPU 掩码类型 (位掩码，每一位代表一个 CPU)
typedef uint64_t cpu_mask_t;

// 获取系统 CPU 数量
// 返回: CPU 数量，失败返回 0
int cpu_affinity_get_cpu_count(void);

// 检查 CPU 是否有效
// cpu_id: CPU ID
// 返回: 有效返回 true，无效返回 false
bool cpu_affinity_is_cpu_valid(int cpu_id);

// 设置当前线程的 CPU 亲和性
// cpu_id: CPU ID
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_set(int cpu_id, cpu_affinity_error_t *error);

// 设置指定线程的 CPU 亲和性
// thread_id: 线程 ID
// cpu_id: CPU ID
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_set_thread(uint64_t thread_id, int cpu_id, cpu_affinity_error_t *error);

// 获取当前线程的 CPU 亲和性
// cpu_id: CPU ID（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_get(int *cpu_id, cpu_affinity_error_t *error);

// 获取指定线程的 CPU 亲和性
// thread_id: 线程 ID
// cpu_id: CPU ID（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_get_thread(uint64_t thread_id, int *cpu_id, cpu_affinity_error_t *error);

// 设置当前线程的 CPU 掩码（多 CPU）
// mask: CPU 掩码
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_set_mask(cpu_mask_t mask, cpu_affinity_error_t *error);

// 设置指定线程的 CPU 掩码（多 CPU）
// thread_id: 线程 ID
// mask: CPU 掩码
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_set_thread_mask(uint64_t thread_id, cpu_mask_t mask, cpu_affinity_error_t *error);

// 获取当前线程的 CPU 掩码（多 CPU）
// mask: CPU 掩码（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_get_mask(cpu_mask_t *mask, cpu_affinity_error_t *error);

// 获取指定线程的 CPU 掩码（多 CPU）
// thread_id: 线程 ID
// mask: CPU 掩码（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_affinity_get_thread_mask(uint64_t thread_id, cpu_mask_t *mask, cpu_affinity_error_t *error);

// 获取当前线程 ID
// 返回: 线程 ID，失败返回 0
uint64_t cpu_affinity_get_current_thread_id(void);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* cpu_affinity_strerror(cpu_affinity_error_t error);

#endif // C_UTILS_CPU_AFFINITY_H
