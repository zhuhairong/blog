#define _GNU_SOURCE
#include "cpu_affinity.h"
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// 获取系统 CPU 数量
int cpu_affinity_get_cpu_count(void) {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

// 检查 CPU 是否有效
bool cpu_affinity_is_cpu_valid(int cpu_id) {
    int cpu_count = cpu_affinity_get_cpu_count();
    return cpu_id >= 0 && cpu_id < cpu_count;
}

// 设置当前线程的 CPU 亲和性
bool cpu_affinity_set(int cpu_id, cpu_affinity_error_t *error) {
    if (!cpu_affinity_is_cpu_valid(cpu_id)) {
        if (error) *error = CPU_AFFINITY_ERROR_INVALID_CPU;
        return false;
    }
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    if (error) *error = CPU_AFFINITY_OK;
    return true;
}

// 设置指定线程的 CPU 亲和性
bool cpu_affinity_set_thread(uint64_t thread_id, int cpu_id, cpu_affinity_error_t *error) {
    if (!cpu_affinity_is_cpu_valid(cpu_id)) {
        if (error) *error = CPU_AFFINITY_ERROR_INVALID_CPU;
        return false;
    }
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    
    pthread_t thread = (pthread_t)thread_id;
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    if (error) *error = CPU_AFFINITY_OK;
    return true;
}

// 获取当前线程的 CPU 亲和性
bool cpu_affinity_get(int *cpu_id, cpu_affinity_error_t *error) {
    if (!cpu_id) {
        if (error) *error = CPU_AFFINITY_ERROR_INVALID_CPU;
        return false;
    }
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    // 找到第一个设置的 CPU
    int cpu_count = cpu_affinity_get_cpu_count();
    for (int i = 0; i < cpu_count; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            *cpu_id = i;
            if (error) *error = CPU_AFFINITY_OK;
            return true;
        }
    }
    
    if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
    return false;
}

// 获取指定线程的 CPU 亲和性
bool cpu_affinity_get_thread(uint64_t thread_id, int *cpu_id, cpu_affinity_error_t *error) {
    if (!cpu_id) {
        if (error) *error = CPU_AFFINITY_ERROR_INVALID_CPU;
        return false;
    }
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    pthread_t thread = (pthread_t)thread_id;
    if (pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    // 找到第一个设置的 CPU
    int cpu_count = cpu_affinity_get_cpu_count();
    for (int i = 0; i < cpu_count; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            *cpu_id = i;
            if (error) *error = CPU_AFFINITY_OK;
            return true;
        }
    }
    
    if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
    return false;
}

// 设置当前线程的 CPU 掩码（多 CPU）
bool cpu_affinity_set_mask(cpu_mask_t mask, cpu_affinity_error_t *error) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    int cpu_count = cpu_affinity_get_cpu_count();
    for (int i = 0; i < cpu_count && i < 64; i++) {
        if (mask & ((cpu_mask_t)1 << i)) {
            CPU_SET(i, &cpuset);
        }
    }
    
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    if (error) *error = CPU_AFFINITY_OK;
    return true;
}

// 设置指定线程的 CPU 掩码（多 CPU）
bool cpu_affinity_set_thread_mask(uint64_t thread_id, cpu_mask_t mask, cpu_affinity_error_t *error) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    int cpu_count = cpu_affinity_get_cpu_count();
    for (int i = 0; i < cpu_count && i < 64; i++) {
        if (mask & ((cpu_mask_t)1 << i)) {
            CPU_SET(i, &cpuset);
        }
    }
    
    pthread_t thread = (pthread_t)thread_id;
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    if (error) *error = CPU_AFFINITY_OK;
    return true;
}

// 获取当前线程的 CPU 掩码（多 CPU）
bool cpu_affinity_get_mask(cpu_mask_t *mask, cpu_affinity_error_t *error) {
    if (!mask) {
        if (error) *error = CPU_AFFINITY_ERROR_INVALID_CPU;
        return false;
    }
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    *mask = 0;
    int cpu_count = cpu_affinity_get_cpu_count();
    for (int i = 0; i < cpu_count && i < 64; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            *mask |= ((cpu_mask_t)1 << i);
        }
    }
    
    if (error) *error = CPU_AFFINITY_OK;
    return true;
}

// 获取指定线程的 CPU 掩码（多 CPU）
bool cpu_affinity_get_thread_mask(uint64_t thread_id, cpu_mask_t *mask, cpu_affinity_error_t *error) {
    if (!mask) {
        if (error) *error = CPU_AFFINITY_ERROR_INVALID_CPU;
        return false;
    }
    
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    pthread_t thread = (pthread_t)thread_id;
    if (pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        if (error) *error = CPU_AFFINITY_ERROR_OPERATION_FAILED;
        return false;
    }
    
    *mask = 0;
    int cpu_count = cpu_affinity_get_cpu_count();
    for (int i = 0; i < cpu_count && i < 64; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            *mask |= ((cpu_mask_t)1 << i);
        }
    }
    
    if (error) *error = CPU_AFFINITY_OK;
    return true;
}

// 获取错误信息
const char* cpu_affinity_error_string(cpu_affinity_error_t error) {
    switch (error) {
        case CPU_AFFINITY_OK:
            return "Success";
        case CPU_AFFINITY_ERROR_INVALID_CPU:
            return "Invalid CPU ID";
        case CPU_AFFINITY_ERROR_INVALID_THREAD:
            return "Invalid thread ID";
        case CPU_AFFINITY_ERROR_OPERATION_FAILED:
            return "Operation failed";
        case CPU_AFFINITY_ERROR_PLATFORM_UNSUPPORTED:
            return "Platform not supported";
        case CPU_AFFINITY_ERROR_MEMORY_ALLOC:
            return "Memory allocation failed";
        default:
            return "Unknown error";
    }
}
