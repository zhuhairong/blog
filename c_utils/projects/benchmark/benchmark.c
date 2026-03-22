#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#include "stopwatch.h"
#include "stats.h"
#include "terminal.h"
#include "json.h"

#define MAX_BENCHMARK_NAME 128
#define MAX_RESULTS 1000
#define MAX_SAMPLES 10000
#define DEFAULT_WARMUP_ITERATIONS 3
#define DEFAULT_TEST_ITERATIONS 10

typedef enum {
    BENCH_OP_NONE = 0,
    BENCH_OP_CPU_INTENSIVE = 1,
    BENCH_OP_MEMORY_ALLOC = 2,
    BENCH_OP_FILE_IO = 3,
    BENCH_OP_MATH = 4,
    BENCH_OP_STRING = 5,
    BENCH_OP_CUSTOM = 6
} benchmark_op_type_t;

typedef struct {
    char name[MAX_BENCHMARK_NAME];
    double *samples;
    size_t sample_count;
    size_t sample_capacity;
    
    double min;
    double max;
    double mean;
    double median;
    double stddev;
    double variance;
    double p50;
    double p75;
    double p90;
    double p95;
    double p99;
    
    uint64_t total_iterations;
    double total_time_ms;
    double ops_per_second;
    
    size_t memory_before;
    size_t memory_after;
    size_t memory_peak;
    
    bool passed;
    char error_msg[256];
} benchmark_result_t;

typedef struct {
    benchmark_result_t **results;
    size_t count;
    size_t capacity;
    char *json_output;
} benchmark_suite_t;

typedef void (*benchmark_func_t)(void *data);

typedef struct {
    const char *name;
    benchmark_func_t func;
    void *data;
    size_t iterations;
    size_t warmup;
} benchmark_case_t;

static uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

static size_t get_memory_usage(void) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return (size_t)usage.ru_maxrss * 1024;
}

static benchmark_result_t* result_create(const char *name) {
    benchmark_result_t *result = calloc(1, sizeof(benchmark_result_t));
    if (!result) return NULL;
    
    strncpy(result->name, name, MAX_BENCHMARK_NAME - 1);
    result->sample_capacity = MAX_SAMPLES;
    result->samples = malloc(result->sample_capacity * sizeof(double));
    if (!result->samples) {
        free(result);
        return NULL;
    }
    
    result->passed = true;
    return result;
}

static void result_free(benchmark_result_t *result) {
    if (result) {
        free(result->samples);
        free(result);
    }
}

static void result_add_sample(benchmark_result_t *result, double sample) {
    if (result->sample_count >= result->sample_capacity) {
        size_t new_capacity = result->sample_capacity * 2;
        double *new_samples = realloc(result->samples, new_capacity * sizeof(double));
        if (!new_samples) return;
        result->samples = new_samples;
        result->sample_capacity = new_capacity;
    }
    result->samples[result->sample_count++] = sample;
}

static int compare_doubles(const void *a, const void *b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}

static void result_compute_stats(benchmark_result_t *result) {
    if (result->sample_count == 0) return;
    
    qsort(result->samples, result->sample_count, sizeof(double), compare_doubles);
    
    double sum = 0;
    result->min = result->samples[0];
    result->max = result->samples[result->sample_count - 1];
    
    for (size_t i = 0; i < result->sample_count; i++) {
        sum += result->samples[i];
    }
    
    result->mean = sum / result->sample_count;
    
    double sq_sum = 0;
    for (size_t i = 0; i < result->sample_count; i++) {
        double diff = result->samples[i] - result->mean;
        sq_sum += diff * diff;
    }
    
    result->variance = sq_sum / result->sample_count;
    result->stddev = sqrt(result->variance);
    
    result->median = result->samples[result->sample_count / 2];
    
    result->p50 = result->median;
    result->p75 = result->samples[(size_t)(result->sample_count * 0.75)];
    result->p90 = result->samples[(size_t)(result->sample_count * 0.90)];
    result->p95 = result->samples[(size_t)(result->sample_count * 0.95)];
    result->p99 = result->samples[(size_t)(result->sample_count * 0.99)];
    
    if (result->total_time_ms > 0) {
        result->ops_per_second = (double)result->total_iterations / (result->total_time_ms / 1000.0);
    }
}

static benchmark_suite_t* suite_create(void) {
    benchmark_suite_t *suite = calloc(1, sizeof(benchmark_suite_t));
    if (!suite) return NULL;
    
    suite->capacity = MAX_RESULTS;
    suite->results = malloc(suite->capacity * sizeof(benchmark_result_t*));
    if (!suite->results) {
        free(suite);
        return NULL;
    }
    
    return suite;
}

static void suite_free(benchmark_suite_t *suite) {
    if (suite) {
        for (size_t i = 0; i < suite->count; i++) {
            result_free(suite->results[i]);
        }
        free(suite->results);
        free(suite->json_output);
        free(suite);
    }
}

static void suite_add_result(benchmark_suite_t *suite, benchmark_result_t *result) {
    if (suite->count >= suite->capacity) {
        size_t new_capacity = suite->capacity * 2;
        benchmark_result_t **new_results = realloc(suite->results, 
                                                    new_capacity * sizeof(benchmark_result_t*));
        if (!new_results) return;
        suite->results = new_results;
        suite->capacity = new_capacity;
    }
    suite->results[suite->count++] = result;
}

static benchmark_result_t* run_benchmark(const char *name, benchmark_func_t func, 
                                         void *data, size_t iterations, size_t warmup) {
    benchmark_result_t *result = result_create(name);
    if (!result) return NULL;
    
    result->memory_before = get_memory_usage();
    result->total_iterations = iterations;
    
    for (size_t w = 0; w < warmup; w++) {
        func(data);
    }
    
    stopwatch_t sw;
    
    for (size_t i = 0; i < iterations; i++) {
        stopwatch_reset(&sw);
        stopwatch_start(&sw);
        
        func(data);
        
        stopwatch_stop(&sw);
        double elapsed_ms = (double)stopwatch_elapsed_ms(&sw);
        result_add_sample(result, elapsed_ms);
        result->total_time_ms += elapsed_ms;
        
        size_t current_mem = get_memory_usage();
        if (current_mem > result->memory_peak) {
            result->memory_peak = current_mem;
        }
    }
    
    result->memory_after = get_memory_usage();
    result_compute_stats(result);
    
    return result;
}

static void bench_cpu_intensive(void *data) {
    volatile double result = 0;
    size_t n = data ? *(size_t*)data : 10000;
    
    for (size_t i = 0; i < n; i++) {
        result += sqrt((double)i) * sin((double)i) * cos((double)i);
    }
}

static void bench_memory_alloc(void *data) {
    size_t block_size = data ? *(size_t*)data : 1024;
    size_t count = 100;
    
    void **ptrs = malloc(count * sizeof(void*));
    if (!ptrs) return;
    
    for (size_t i = 0; i < count; i++) {
        ptrs[i] = malloc(block_size);
        if (ptrs[i]) {
            memset(ptrs[i], 0, block_size);
        }
    }
    
    for (size_t i = 0; i < count; i++) {
        free(ptrs[i]);
    }
    
    free(ptrs);
}

static void bench_math_operations(void *data) {
    size_t n = data ? *(size_t*)data : 10000;
    volatile double result = 0;
    
    for (size_t i = 1; i <= n; i++) {
        result += log((double)i);
        result += exp((double)i / 1000.0);
        result += pow((double)i, 0.5);
    }
}

static void bench_string_operations(void *data) {
    size_t n = data ? *(size_t*)data : 1000;
    char buf[256];
    
    for (size_t i = 0; i < n; i++) {
        snprintf(buf, sizeof(buf), "test_string_%zu_value_%zu", i, i * 2);
        size_t len = strlen(buf);
        for (size_t j = 0; j < len; j++) {
            buf[j] = toupper(buf[j]);
        }
    }
}

static void bench_file_io(void *data) {
    const char *filename = data ? (const char*)data : "/tmp/benchmark_test.tmp";
    
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;
    
    char buffer[4096];
    memset(buffer, 'A', sizeof(buffer));
    
    for (int i = 0; i < 10; i++) {
        fwrite(buffer, 1, sizeof(buffer), fp);
    }
    fclose(fp);
    
    fp = fopen(filename, "rb");
    if (fp) {
        while (fread(buffer, 1, sizeof(buffer), fp) > 0);
        fclose(fp);
    }
    
    remove(filename);
}

static void run_builtin_benchmarks(benchmark_suite_t *suite, size_t iterations, size_t warmup) {
    size_t cpu_n = 50000;
    size_t mem_size = 4096;
    size_t math_n = 10000;
    size_t str_n = 5000;
    
    printf("运行内置基准测试...\n\n");
    
    printf("[1/4] CPU密集型操作...\n");
    benchmark_result_t *cpu_result = run_benchmark("CPU密集型", bench_cpu_intensive, 
                                                    &cpu_n, iterations, warmup);
    if (cpu_result) suite_add_result(suite, cpu_result);
    
    printf("[2/4] 内存分配操作...\n");
    benchmark_result_t *mem_result = run_benchmark("内存分配", bench_memory_alloc, 
                                                    &mem_size, iterations, warmup);
    if (mem_result) suite_add_result(suite, mem_result);
    
    printf("[3/4] 数学运算操作...\n");
    benchmark_result_t *math_result = run_benchmark("数学运算", bench_math_operations, 
                                                     &math_n, iterations, warmup);
    if (math_result) suite_add_result(suite, math_result);
    
    printf("[4/4] 字符串操作...\n");
    benchmark_result_t *str_result = run_benchmark("字符串操作", bench_string_operations, 
                                                    &str_n, iterations, warmup);
    if (str_result) suite_add_result(suite, str_result);
}

static void print_result_table(benchmark_suite_t *suite) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                              性能基准测试结果报告                                    \n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    printf("%-20s %10s %10s %10s %10s %10s %10s\n", 
           "测试名称", "平均(ms)", "最小(ms)", "最大(ms)", "标准差", "P99(ms)", "ops/s");
    printf("───────────────────────────────────────────────────────────────────────────────────────\n");
    
    for (size_t i = 0; i < suite->count; i++) {
        benchmark_result_t *r = suite->results[i];
        
        const char *color = TERM_ANSI_RESET;
        if (r->passed) {
            color = TERM_ANSI_GREEN;
        } else {
            color = TERM_ANSI_RED;
        }
        
        printf("%s%-20s %10.3f %10.3f %10.3f %10.3f %10.3f %10.0f%s\n",
               color, r->name, r->mean, r->min, r->max, r->stddev, r->p99, r->ops_per_second,
               TERM_ANSI_RESET);
    }
    
    printf("───────────────────────────────────────────────────────────────────────────────────────\n");
}

static void print_detailed_result(benchmark_result_t *r) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "───────────────────────────────────────────────────────────────────────────────────────\n");
    printf("测试名称: %s\n", r->name);
    term_printf(TERM_ANSI_CYAN, "───────────────────────────────────────────────────────────────────────────────────────\n");
    
    printf("\n时间统计:\n");
    printf("  总迭代次数: %lu\n", r->total_iterations);
    printf("  总耗时: %.3f ms\n", r->total_time_ms);
    printf("  平均耗时: %.3f ms\n", r->mean);
    printf("  最小耗时: %.3f ms\n", r->min);
    printf("  最大耗时: %.3f ms\n", r->max);
    printf("  标准差: %.3f ms\n", r->stddev);
    printf("  方差: %.3f ms²\n", r->variance);
    printf("  吞吐量: %.0f ops/s\n", r->ops_per_second);
    
    printf("\n百分位数:\n");
    printf("  P50 (中位数): %.3f ms\n", r->p50);
    printf("  P75: %.3f ms\n", r->p75);
    printf("  P90: %.3f ms\n", r->p90);
    printf("  P95: %.3f ms\n", r->p95);
    printf("  P99: %.3f ms\n", r->p99);
    
    printf("\n内存使用:\n");
    printf("  测试前: %.2f MB\n", (double)r->memory_before / (1024 * 1024));
    printf("  测试后: %.2f MB\n", (double)r->memory_after / (1024 * 1024));
    printf("  峰值: %.2f MB\n", (double)r->memory_peak / (1024 * 1024));
    printf("  增长: %.2f MB\n", (double)(r->memory_after - r->memory_before) / (1024 * 1024));
}

static void print_detailed_results(benchmark_suite_t *suite) {
    for (size_t i = 0; i < suite->count; i++) {
        print_detailed_result(suite->results[i]);
    }
}

static char* generate_json_report(benchmark_suite_t *suite) {
    size_t buf_size = 1024 * 1024;
    char *json = malloc(buf_size);
    if (!json) return NULL;
    
    size_t pos = 0;
    pos += snprintf(json + pos, buf_size - pos, "{\n  \"results\": [\n");
    
    for (size_t i = 0; i < suite->count; i++) {
        benchmark_result_t *r = suite->results[i];
        
        pos += snprintf(json + pos, buf_size - pos,
            "    {\n"
            "      \"name\": \"%s\",\n"
            "      \"iterations\": %lu,\n"
            "      \"total_time_ms\": %.3f,\n"
            "      \"mean_ms\": %.3f,\n"
            "      \"min_ms\": %.3f,\n"
            "      \"max_ms\": %.3f,\n"
            "      \"stddev_ms\": %.3f,\n"
            "      \"variance_ms2\": %.3f,\n"
            "      \"median_ms\": %.3f,\n"
            "      \"p50_ms\": %.3f,\n"
            "      \"p75_ms\": %.3f,\n"
            "      \"p90_ms\": %.3f,\n"
            "      \"p95_ms\": %.3f,\n"
            "      \"p99_ms\": %.3f,\n"
            "      \"ops_per_second\": %.0f,\n"
            "      \"memory_before_bytes\": %zu,\n"
            "      \"memory_after_bytes\": %zu,\n"
            "      \"memory_peak_bytes\": %zu,\n"
            "      \"passed\": %s\n"
            "    }%s\n",
            r->name, r->total_iterations, r->total_time_ms,
            r->mean, r->min, r->max, r->stddev, r->variance, r->median,
            r->p50, r->p75, r->p90, r->p95, r->p99, r->ops_per_second,
            r->memory_before, r->memory_after, r->memory_peak,
            r->passed ? "true" : "false",
            (i < suite->count - 1) ? "," : "");
    }
    
    pos += snprintf(json + pos, buf_size - pos, 
        "  ],\n"
        "  \"summary\": {\n"
        "    \"total_tests\": %zu,\n"
        "    \"passed\": %zu,\n"
        "    \"failed\": %zu\n"
        "  }\n"
        "}\n",
        suite->count,
        suite->count,
        0LU);
    
    return json;
}

static bool save_json_report(const char *filename, benchmark_suite_t *suite) {
    char *json = generate_json_report(suite);
    if (!json) return false;
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        free(json);
        return false;
    }
    
    fputs(json, fp);
    fclose(fp);
    free(json);
    return true;
}

static void print_system_info(void) {
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                              系统信息                                                \n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    printf("CPU核心数: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf("页面大小: %ld bytes\n", sysconf(_SC_PAGESIZE));
    printf("物理页数: %ld\n", sysconf(_SC_PHYS_PAGES));
    printf("总内存: %.2f GB\n", 
           (double)sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE) / (1024 * 1024 * 1024));
    printf("可用内存: %.2f GB\n", 
           (double)sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) / (1024 * 1024 * 1024));
    
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("当前进程内存: %.2f MB\n", (double)usage.ru_maxrss / 1024);
}

static void print_help(const char *prog) {
    term_printf(TERM_ANSI_CYAN, "\n性能基准测试工具 v1.0\n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════════════════════════════\n\n");
    
    printf("用法: %s [选项]\n\n", prog);
    
    printf("选项:\n");
    printf("  -i, --iterations <num>   测试迭代次数 (默认: %d)\n", DEFAULT_TEST_ITERATIONS);
    printf("  -w, --warmup <num>       预热迭代次数 (默认: %d)\n", DEFAULT_WARMUP_ITERATIONS);
    printf("  -o, --output <file>      输出JSON报告文件\n");
    printf("  -v, --verbose            详细输出模式\n");
    printf("  -s, --system             显示系统信息\n");
    printf("  -h, --help               显示帮助信息\n");
    
    printf("\n内置基准测试:\n");
    printf("  CPU密集型    - 数学计算和三角函数\n");
    printf("  内存分配     - 内存分配和释放\n");
    printf("  数学运算     - 对数、指数、幂运算\n");
    printf("  字符串操作   - 字符串格式化和处理\n");
    
    printf("\n示例:\n");
    printf("  %s                          # 运行默认基准测试\n", prog);
    printf("  %s -i 100 -w 5              # 100次迭代，5次预热\n", prog);
    printf("  %s -o report.json           # 输出JSON报告\n", prog);
    printf("  %s -v                       # 详细输出\n", prog);
}

int main(int argc, char **argv) {
    size_t iterations = DEFAULT_TEST_ITERATIONS;
    size_t warmup = DEFAULT_WARMUP_ITERATIONS;
    const char *output_file = NULL;
    bool verbose = false;
    bool show_system = false;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations") == 0) {
            if (i + 1 < argc) {
                iterations = (size_t)atoi(argv[++i]);
                if (iterations == 0) iterations = DEFAULT_TEST_ITERATIONS;
            }
        } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--warmup") == 0) {
            if (i + 1 < argc) {
                warmup = (size_t)atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_file = argv[++i];
            }
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--system") == 0) {
            show_system = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        }
    }
    
    if (show_system) {
        print_system_info();
        return 0;
    }
    
    printf("\n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════════════════════════════\n");
    term_printf(TERM_ANSI_CYAN, "                         性能基准测试工具 v1.0                                          \n");
    term_printf(TERM_ANSI_CYAN, "═══════════════════════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    printf("配置:\n");
    printf("  迭代次数: %zu\n", iterations);
    printf("  预热次数: %zu\n", warmup);
    printf("  详细模式: %s\n", verbose ? "是" : "否");
    if (output_file) {
        printf("  输出文件: %s\n", output_file);
    }
    
    print_system_info();
    
    benchmark_suite_t *suite = suite_create();
    if (!suite) {
        fprintf(stderr, "错误: 无法创建测试套件\n");
        return 1;
    }
    
    run_builtin_benchmarks(suite, iterations, warmup);
    
    print_result_table(suite);
    
    if (verbose) {
        print_detailed_results(suite);
    }
    
    if (output_file) {
        if (save_json_report(output_file, suite)) {
            printf("\nJSON报告已保存到: %s\n", output_file);
        } else {
            fprintf(stderr, "错误: 无法保存JSON报告\n");
        }
    }
    
    size_t passed = 0;
    for (size_t i = 0; i < suite->count; i++) {
        if (suite->results[i]->passed) passed++;
    }
    
    printf("\n");
    if (passed == suite->count) {
        term_printf(TERM_ANSI_GREEN, "✓ 所有 %zu 项测试通过！\n", suite->count);
    } else {
        term_printf(TERM_ANSI_YELLOW, "⚠ %zu/%zu 项测试通过\n", passed, suite->count);
    }
    
    suite_free(suite);
    return 0;
}
