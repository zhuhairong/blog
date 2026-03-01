#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#define _GNU_SOURCE
#include "regex_tiny.h"
#include "csv.h"
#include "stats.h"
#include "hashmap.h"
#include "time_utils.h"
#include "argparse.h"
#include "fs_utils.h"
#include "sort.h"

#define MAX_LINE_SIZE 4096
#define MAX_PATTERN_SIZE 256
#define MAX_ERROR_TYPES 100
#define MAX_PATH_SIZE 1024

typedef struct {
    char error_type[MAX_PATTERN_SIZE];
    size_t count;
    time_t first_seen;
    time_t last_seen;
} error_stat_t;

typedef struct {
    char level[32];
    size_t count;
} level_stat_t;

typedef struct {
    char hour[8];
    size_t count;
} hourly_stat_t;

typedef struct {
    char pattern[MAX_PATTERN_SIZE];
    char description[256];
} log_pattern_t;

typedef struct {
    error_stat_t errors[MAX_ERROR_TYPES];
    size_t error_count;
    level_stat_t levels[10];
    size_t level_count;
    hourly_stat_t hourly[24];
    size_t total_lines;
    size_t matched_lines;
    time_t start_time;
    time_t end_time;
    char filename[MAX_PATH_SIZE];
} log_analysis_result_t;

static log_pattern_t common_patterns[] = {
    {"ERROR", "错误级别日志"},
    {"WARN", "警告级别日志"},
    {"INFO", "信息级别日志"},
    {"DEBUG", "调试级别日志"},
    {"Exception", "异常信息"},
    {"Failed", "失败操作"},
    {"Timeout", "超时错误"},
    {"Connection refused", "连接被拒绝"},
    {"Out of memory", "内存不足"},
    {"Segmentation fault", "段错误"},
    {"Stack overflow", "栈溢出"},
    {"Access denied", "访问被拒绝"},
    {"File not found", "文件未找到"},
    {"Permission denied", "权限被拒绝"},
    {"NULL pointer", "空指针"}
};

static int pattern_count = sizeof(common_patterns) / sizeof(common_patterns[0]);

static void init_result(log_analysis_result_t *result) {
    memset(result, 0, sizeof(log_analysis_result_t));
    for (int i = 0; i < 24; i++) {
        snprintf(result->hourly[i].hour, sizeof(result->hourly[i].hour), "%02d:00", i);
    }
}

static int find_or_add_error(log_analysis_result_t *result, const char *error_type) {
    for (size_t i = 0; i < result->error_count; i++) {
        if (strcmp(result->errors[i].error_type, error_type) == 0) {
            return (int)i;
        }
    }
    if (result->error_count < MAX_ERROR_TYPES) {
        strncpy(result->errors[result->error_count].error_type, error_type, MAX_PATTERN_SIZE - 1);
        result->errors[result->error_count].count = 0;
        result->error_count++;
        return (int)(result->error_count - 1);
    }
    return -1;
}

static int find_or_add_level(log_analysis_result_t *result, const char *level) {
    for (size_t i = 0; i < result->level_count; i++) {
        if (strcmp(result->levels[i].level, level) == 0) {
            return (int)i;
        }
    }
    if (result->level_count < 10) {
        strncpy(result->levels[result->level_count].level, level, sizeof(result->levels[0].level) - 1);
        result->levels[result->level_count].count = 0;
        result->level_count++;
        return (int)(result->level_count - 1);
    }
    return -1;
}

static bool parse_timestamp(const char *line, time_t *timestamp, int *hour) {
    struct tm tm = {0};
    char *result = NULL;
    
    result = strptime(line, "%Y-%m-%d %H:%M:%S", &tm);
    if (result) {
        *timestamp = mktime(&tm);
        *hour = tm.tm_hour;
        return true;
    }
    
    result = strptime(line, "%d/%b/%Y:%H:%M:%S", &tm);
    if (result) {
        *timestamp = mktime(&tm);
        *hour = tm.tm_hour;
        return true;
    }
    
    result = strptime(line, "%b %d %H:%M:%S", &tm);
    if (result) {
        tm.tm_year = 2024 - 1900;
        *timestamp = mktime(&tm);
        *hour = tm.tm_hour;
        return true;
    }
    
    return false;
}

static bool extract_log_level(const char *line, char *level, size_t level_size) {
    const char *levels[] = {"ERROR", "WARN", "WARNING", "INFO", "DEBUG", "TRACE", "FATAL", "CRITICAL"};
    for (int i = 0; i < 8; i++) {
        if (strstr(line, levels[i])) {
            strncpy(level, levels[i], level_size - 1);
            level[level_size - 1] = '\0';
            return true;
        }
    }
    return false;
}

static bool case_insensitive_match(const char *pattern, const char *text) {
    char pattern_lower[MAX_PATTERN_SIZE] = {0};
    char text_lower[MAX_LINE_SIZE] = {0};
    
    for (int i = 0; pattern[i] && i < MAX_PATTERN_SIZE - 1; i++) {
        pattern_lower[i] = tolower((unsigned char)pattern[i]);
    }
    
    for (int i = 0; text[i] && i < MAX_LINE_SIZE - 1; i++) {
        text_lower[i] = tolower((unsigned char)text[i]);
    }
    
    return regex_match(pattern_lower, text_lower);
}

static bool contains_pattern(const char *line, const char *pattern) {
    if (strstr(line, pattern)) {
        return true;
    }
    return case_insensitive_match(pattern, line);
}

static void analyze_line(const char *line, log_analysis_result_t *result) {
    result->total_lines++;
    
    time_t timestamp = 0;
    int hour = 0;
    bool has_timestamp = parse_timestamp(line, &timestamp, &hour);
    
    if (has_timestamp) {
        if (result->start_time == 0 || timestamp < result->start_time) {
            result->start_time = timestamp;
        }
        if (timestamp > result->end_time) {
            result->end_time = timestamp;
        }
        result->hourly[hour].count++;
    }
    
    char level[32] = {0};
    if (extract_log_level(line, level, sizeof(level))) {
        int idx = find_or_add_level(result, level);
        if (idx >= 0) {
            result->levels[idx].count++;
        }
    }
    
    for (int i = 0; i < pattern_count; i++) {
        if (contains_pattern(line, common_patterns[i].pattern)) {
            int idx = find_or_add_error(result, common_patterns[i].pattern);
            if (idx >= 0) {
                result->errors[idx].count++;
                if (has_timestamp) {
                    if (result->errors[idx].first_seen == 0 || timestamp < result->errors[idx].first_seen) {
                        result->errors[idx].first_seen = timestamp;
                    }
                    if (timestamp > result->errors[idx].last_seen) {
                        result->errors[idx].last_seen = timestamp;
                    }
                }
            }
            result->matched_lines++;
            break;
        }
    }
}

static int analyze_log_file(const char *filename, log_analysis_result_t *result) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "无法打开文件: %s\n", filename);
        return -1;
    }
    
    strncpy(result->filename, filename, MAX_PATH_SIZE - 1);
    
    char line[MAX_LINE_SIZE];
    while (fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        analyze_line(line, result);
    }
    
    fclose(fp);
    return 0;
}

static void print_report(const log_analysis_result_t *result) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    日志分析报告                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("📁 文件: %s\n", result->filename);
    printf("📊 总行数: %zu\n", result->total_lines);
    printf("✅ 匹配行数: %zu\n", result->matched_lines);
    printf("📈 匹配率: %.2f%%\n", 
           result->total_lines > 0 ? (double)result->matched_lines / result->total_lines * 100 : 0);
    
    if (result->start_time > 0 && result->end_time > 0) {
        char start_str[64] = {0};
        char end_str[64] = {0};
        struct tm *tm_info = localtime(&result->start_time);
        strftime(start_str, sizeof(start_str), "%Y-%m-%d %H:%M:%S", tm_info);
        tm_info = localtime(&result->end_time);
        strftime(end_str, sizeof(end_str), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("⏰ 时间范围: %s ~ %s\n", start_str, end_str);
        printf("⏱️  持续时间: %ld 秒\n", result->end_time - result->start_time);
    }
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                    日志级别统计                              │\n");
    printf("├──────────────┬──────────────┬────────────────────────────────┤\n");
    printf("│     级别     │     数量     │            占比                │\n");
    printf("├──────────────┼──────────────┼────────────────────────────────┤\n");
    
    for (size_t i = 0; i < result->level_count; i++) {
        double percentage = result->total_lines > 0 ? 
            (double)result->levels[i].count / result->total_lines * 100 : 0;
        printf("│ %-12s │ %12zu │ %26.2f%% │\n", 
               result->levels[i].level, result->levels[i].count, percentage);
    }
    printf("└──────────────┴──────────────┴────────────────────────────────┘\n");
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                    错误类型统计                              │\n");
    printf("├────────────────────────┬──────────┬─────────────────────────┤\n");
    printf("│       错误类型         │   数量   │         描述            │\n");
    printf("├────────────────────────┼──────────┼─────────────────────────┤\n");
    
    for (size_t i = 0; i < result->error_count && i < 15; i++) {
        const char *desc = "";
        for (int j = 0; j < pattern_count; j++) {
            if (strcmp(result->errors[i].error_type, common_patterns[j].pattern) == 0) {
                desc = common_patterns[j].description;
                break;
            }
        }
        printf("│ %-22s │ %8zu │ %-23s │\n", 
               result->errors[i].error_type, result->errors[i].count, desc);
    }
    printf("└────────────────────────┴──────────┴─────────────────────────┘\n");
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                    每小时分布                                │\n");
    printf("├────────┬────────────────────────────────────────────────────┤\n");
    printf("│  时间  │                    分布图                          │\n");
    printf("├────────┼────────────────────────────────────────────────────┤\n");
    
    size_t max_count = 0;
    for (int i = 0; i < 24; i++) {
        if (result->hourly[i].count > max_count) {
            max_count = result->hourly[i].count;
        }
    }
    
    for (int i = 0; i < 24; i++) {
        printf("│ %s │ ", result->hourly[i].hour);
        int bar_len = max_count > 0 ? (int)(result->hourly[i].count * 40 / max_count) : 0;
        for (int j = 0; j < bar_len; j++) {
            printf("█");
        }
        printf(" %zu\n", result->hourly[i].count);
    }
    printf("└────────┴────────────────────────────────────────────────────┘\n");
}

static int export_csv(const log_analysis_result_t *result, const char *output_file) {
    csv_t *csv = csv_create(NULL, NULL);
    if (!csv) {
        fprintf(stderr, "创建CSV失败\n");
        return -1;
    }
    
    const char *header[] = {"错误类型", "数量", "描述"};
    csv_add_row(csv, header, 3, NULL);
    
    for (size_t i = 0; i < result->error_count; i++) {
        const char *desc = "";
        for (int j = 0; j < pattern_count; j++) {
            if (strcmp(result->errors[i].error_type, common_patterns[j].pattern) == 0) {
                desc = common_patterns[j].description;
                break;
            }
        }
        char count_str[32];
        snprintf(count_str, sizeof(count_str), "%zu", result->errors[i].count);
        const char *row[] = {
            result->errors[i].error_type,
            count_str,
            desc
        };
        csv_add_row(csv, row, 3, NULL);
    }
    
    csv_error_t error;
    bool success = csv_save(csv, output_file, &error);
    csv_free(csv);
    
    if (success) {
        printf("CSV报告已保存到: %s\n", output_file);
        return 0;
    } else {
        fprintf(stderr, "保存CSV失败\n");
        return -1;
    }
}

static void search_in_file(const char *filename, const char *pattern) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "无法打开文件: %s\n", filename);
        return;
    }
    
    printf("\n搜索结果 (模式: %s):\n", pattern);
    printf("═══════════════════════════════════════════════════════════\n");
    
    char line[MAX_LINE_SIZE];
    int line_num = 0;
    int match_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        if (contains_pattern(line, pattern)) {
            printf("行 %d: %s\n", line_num, line);
            match_count++;
        }
    }
    
    fclose(fp);
    printf("═══════════════════════════════════════════════════════════\n");
    printf("共找到 %d 个匹配\n", match_count);
}

static void print_usage(const char *program) {
    printf("用法: %s [选项] <日志文件>\n\n", program);
    printf("选项:\n");
    printf("  -h, --help           显示帮助信息\n");
    printf("  -o, --output <文件>  导出CSV报告\n");
    printf("  -s, --search <模式>  搜索指定模式\n");
    printf("  -p, --pattern <模式> 添加自定义错误模式\n");
    printf("\n示例:\n");
    printf("  %s app.log                    # 分析日志文件\n", program);
    printf("  %s -o report.csv app.log      # 分析并导出CSV\n", program);
    printf("  %s -s \"ERROR\" app.log         # 搜索包含ERROR的行\n", program);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    const char *log_file = NULL;
    const char *output_file = NULL;
    const char *search_pattern = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_file = argv[++i];
            }
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--search") == 0) {
            if (i + 1 < argc) {
                search_pattern = argv[++i];
            }
        } else if (argv[i][0] != '-') {
            log_file = argv[i];
        }
    }
    
    if (!log_file) {
        fprintf(stderr, "错误: 未指定日志文件\n");
        print_usage(argv[0]);
        return 1;
    }
    
    if (search_pattern) {
        search_in_file(log_file, search_pattern);
        return 0;
    }
    
    log_analysis_result_t result;
    init_result(&result);
    
    printf("正在分析日志文件: %s\n", log_file);
    
    if (analyze_log_file(log_file, &result) != 0) {
        return 1;
    }
    
    print_report(&result);
    
    if (output_file) {
        export_csv(&result, output_file);
    }
    
    return 0;
}
