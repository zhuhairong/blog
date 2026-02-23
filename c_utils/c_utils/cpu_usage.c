#include "cpu_usage.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

#ifdef __linux__
#include <sys/sysinfo.h>
#endif

static cpu_time_stats_t last_stats[256] = {0};
static int core_count = 0;
static bool initialized = false;

bool cpu_usage_init(cpu_usage_error_t *error) {
    if (initialized) return true;
    
#ifdef __linux__
    core_count = get_nprocs();
#else
    core_count = 1;
#endif
    
    if (core_count <= 0) {
        if (error) *error = CPU_USAGE_ERROR_OPERATION_FAILED;
        return false;
    }
    
    initialized = true;
    if (error) *error = CPU_USAGE_OK;
    return true;
}

double cpu_usage_get_time(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}

double cpu_usage_get_process_time(double *user_time, double *system_time, cpu_usage_error_t *error) {
    struct tms t;
    clock_t c = times(&t);
    
    if (c == (clock_t)-1) {
        if (error) *error = CPU_USAGE_ERROR_OPERATION_FAILED;
        return 0.0;
    }
    
    double ticks_per_sec = (double)sysconf(_SC_CLK_TCK);
    double user = (double)t.tms_utime / ticks_per_sec;
    double sys = (double)t.tms_stime / ticks_per_sec;
    
    if (user_time) *user_time = user;
    if (system_time) *system_time = sys;
    if (error) *error = CPU_USAGE_OK;
    
    return user + sys;
}

#ifdef __linux__
static bool read_cpu_stats(int core_id, cpu_time_stats_t *stats) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return false;
    
    char line[512];
    bool found = false;
    
    while (fgets(line, sizeof(line), fp)) {
        char cpu_label[16];
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
        
        if (core_id == 0) {
            if (sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                       &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) == 8) {
                stats->user = (double)user;
                stats->system = (double)system;
                stats->idle = (double)idle;
                stats->io_wait = (double)iowait;
                stats->nice = (double)nice;
                stats->irq = (double)irq;
                stats->softirq = (double)softirq;
                stats->steal = (double)steal;
                found = true;
                break;
            }
        } else {
            if (sscanf(line, "%s %llu %llu %llu %llu %llu %llu %llu %llu",
                       cpu_label, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) == 9) {
                if (strncmp(cpu_label, "cpu", 3) == 0) {
                    int id = atoi(cpu_label + 3);
                    if (id == core_id - 1) {
                        stats->user = (double)user;
                        stats->system = (double)system;
                        stats->idle = (double)idle;
                        stats->io_wait = (double)iowait;
                        stats->nice = (double)nice;
                        stats->irq = (double)irq;
                        stats->softirq = (double)softirq;
                        stats->steal = (double)steal;
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    
    fclose(fp);
    return found;
}
#endif

bool cpu_usage_get_overall(cpu_usage_stats_t *stats, cpu_usage_error_t *error) {
    if (!stats) {
        if (error) *error = CPU_USAGE_ERROR_OPERATION_FAILED;
        return false;
    }
    
    if (!initialized) {
        cpu_usage_init(error);
    }
    
#ifdef __linux__
    cpu_time_stats_t current;
    if (!read_cpu_stats(0, &current)) {
        if (error) *error = CPU_USAGE_ERROR_OPERATION_FAILED;
        return false;
    }
    
    cpu_time_stats_t *last = &last_stats[0];
    
    double total_delta = (current.user - last->user) + 
                         (current.nice - last->nice) +
                         (current.system - last->system) + 
                         (current.idle - last->idle) +
                         (current.io_wait - last->io_wait) +
                         (current.irq - last->irq) +
                         (current.softirq - last->softirq) +
                         (current.steal - last->steal);
    
    if (total_delta > 0) {
        stats->user = (current.user - last->user + current.nice - last->nice) / total_delta * 100.0;
        stats->system = (current.system - last->system) / total_delta * 100.0;
        stats->idle = (current.idle - last->idle) / total_delta * 100.0;
        stats->io_wait = (current.io_wait - last->io_wait) / total_delta * 100.0;
        stats->nice = (current.nice - last->nice) / total_delta * 100.0;
        stats->irq = (current.irq - last->irq) / total_delta * 100.0;
        stats->softirq = (current.softirq - last->softirq) / total_delta * 100.0;
        stats->steal = (current.steal - last->steal) / total_delta * 100.0;
        stats->total = 100.0 - stats->idle;
    } else {
        memset(stats, 0, sizeof(*stats));
    }
    
    *last = current;
    if (error) *error = CPU_USAGE_OK;
    return true;
#else
    memset(stats, 0, sizeof(*stats));
    if (error) *error = CPU_USAGE_ERROR_PLATFORM_UNSUPPORTED;
    return false;
#endif
}

bool cpu_usage_get_core(int core_id, cpu_usage_stats_t *stats, cpu_usage_error_t *error) {
    if (!stats || core_id < 0 || core_id > core_count) {
        if (error) *error = CPU_USAGE_ERROR_INVALID_CORE;
        return false;
    }
    
    return cpu_usage_get_overall(stats, error);
}

bool cpu_usage_get_time_stats(int core_id, cpu_time_stats_t *stats, cpu_usage_error_t *error) {
    if (!stats) {
        if (error) *error = CPU_USAGE_ERROR_OPERATION_FAILED;
        return false;
    }
    
#ifdef __linux__
    if (!read_cpu_stats(core_id, stats)) {
        if (error) *error = CPU_USAGE_ERROR_OPERATION_FAILED;
        return false;
    }
    if (error) *error = CPU_USAGE_OK;
    return true;
#else
    if (error) *error = CPU_USAGE_ERROR_PLATFORM_UNSUPPORTED;
    return false;
#endif
}

int cpu_usage_get_core_count(void) {
    if (!initialized) {
        cpu_usage_init(NULL);
    }
    return core_count;
}

bool cpu_usage_is_core_valid(int core_id) {
    return core_id >= 0 && core_id <= core_count;
}

bool cpu_usage_refresh(cpu_usage_error_t *error) {
    if (error) *error = CPU_USAGE_OK;
    return true;
}

const char* cpu_usage_strerror(cpu_usage_error_t error) {
    switch (error) {
        case CPU_USAGE_OK: return "Success";
        case CPU_USAGE_ERROR_INVALID_CORE: return "Invalid CPU core";
        case CPU_USAGE_ERROR_OPERATION_FAILED: return "Operation failed";
        case CPU_USAGE_ERROR_PLATFORM_UNSUPPORTED: return "Platform unsupported";
        case CPU_USAGE_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case CPU_USAGE_ERROR_INSUFFICIENT_DATA: return "Insufficient data";
        default: return "Unknown error";
    }
}
