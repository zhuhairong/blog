/**
 * @file demo_csv.c
 * @brief CSV 解析演示
 *
 * 本演示展示了 CSV 文件的解析和处理，包括：
 * - CSV 文件读取
 * - 行解析
 * - 配置选项
 * - 错误处理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/csv.h"

// 打印 CSV 行
static void print_row(csv_row_t *row) {
    printf("  字段数: %zu\n", row->count);
    printf("  内容: [");
    for (size_t i = 0; i < row->count; i++) {
        if (i > 0) printf(", ");
        printf("\"%s\"", row->fields[i]);
    }
    printf("]\n");
}

// 演示 1: 基本 CSV 解析
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本 CSV 解析 ===\n");

    const char *csv_lines[] = {
        "name,age,city",
        "Alice,30,New York",
        "Bob,25,Los Angeles",
        "\"Charlie, Jr.\",35,\"San Francisco, CA\"",
        "\"Diana\"\"s\",28,Boston"
    };
    int num_lines = sizeof(csv_lines) / sizeof(csv_lines[0]);

    printf("CSV 行解析:\n\n");
    for (int i = 0; i < num_lines; i++) {
        printf("输入: %s\n", csv_lines[i]);
        
        csv_error_t error;
        csv_row_t *row = csv_parse_line(csv_lines[i], NULL, &error);
        
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
        printf("\n");
    }
}

// 演示 2: 不同分隔符
static void demo_delimiters(void) {
    printf("\n=== 演示 2: 不同分隔符 ===\n");

    const char *lines[] = {
        "name;age;city",        // 分号分隔
        "name\tage\tcity",      // Tab 分隔
        "name|age|city",        // 管道分隔
        "name,age,city"         // 逗号分隔
    };
    char delimiters[] = {';', '\t', '|', ','};
    const char *names[] = {"分号", "Tab", "管道", "逗号"};
    
    int num = sizeof(lines) / sizeof(lines[0]);

    for (int i = 0; i < num; i++) {
        printf("%s分隔: %s\n", names[i], lines[i]);
        
        csv_config_t config = csv_default_config();
        config.delimiter = delimiters[i];
        
        csv_error_t error;
        csv_row_t *row = csv_parse_line(lines[i], &config, &error);
        
        if (row) {
            print_row(row);
            csv_row_free(row);
        }
        printf("\n");
    }
}

// 演示 3: 引号处理
static void demo_quotes(void) {
    printf("\n=== 演示 3: 引号处理 ===\n");

    const char *lines[] = {
        "simple text",                      // 无引号
        "\"quoted text\"",                  // 完全引号
        "\"text with \"\"quotes\"\"\"",      // 转义引号
        "\"line1\nline2\"",                 // 多行文本
        "\"  spaces  \""                    // 保留空格
    };
    int num = sizeof(lines) / sizeof(lines[0]);

    printf("引号处理示例:\n\n");
    for (int i = 0; i < num; i++) {
        printf("输入: %s\n", lines[i]);
        
        csv_error_t error;
        csv_row_t *row = csv_parse_line(lines[i], NULL, &error);
        
        if (row) {
            if (row->count > 0) {
                printf("  解析结果: \"%s\"\n", row->fields[0]);
                printf("  长度: %zu\n", strlen(row->fields[0]));
            }
            csv_row_free(row);
        }
        printf("\n");
    }
}

// 演示 4: 文件读取
static void demo_file_read(void) {
    printf("\n=== 演示 4: 文件读取 ===\n");

    // 创建测试文件
    const char *test_file = "/tmp/csv_test.csv";
    FILE *fp = fopen(test_file, "w");
    if (!fp) {
        printf("无法创建测试文件\n");
        return;
    }

    fprintf(fp, "Name,Age,Department,Salary\n");
    fprintf(fp, "Alice,30,Engineering,75000\n");
    fprintf(fp, "Bob,25,Marketing,60000\n");
    fprintf(fp, "\"Carol Smith\",35,\"Human Resources\",80000\n");
    fprintf(fp, "David,28,Engineering,70000\n");
    fclose(fp);

    printf("测试文件: %s\n\n", test_file);

    // 读取整个文件
    csv_error_t error;
    csv_t *csv = csv_load(test_file, NULL, &error);

    if (csv) {
        printf("读取成功:\n");
        printf("  行数: %zu\n", csv_get_rows(csv));
        printf("  列数: %zu\n", csv_get_cols(csv));
        printf("\n数据预览:\n");

        // 打印前 4 行
        size_t max_rows = csv_get_rows(csv) < 5 ? csv_get_rows(csv) : 5;
        for (size_t i = 0; i < max_rows; i++) {
            printf("  行 %zu: [", i);
            size_t cols = csv_get_cols(csv);
            for (size_t j = 0; j < cols && j < 4; j++) {
                if (j > 0) printf(", ");
                const char *val = csv_get(csv, i, j, &error);
                printf("\"%s\"", val ? val : "");
            }
            if (cols > 4) printf(", ...");
            printf("]\n");
        }
        if (csv_get_rows(csv) > 5) {
            printf("  ... 还有 %zu 行\n", csv_get_rows(csv) - 5);
        }

        csv_free(csv);
    } else {
        printf("读取失败，错误码: %d\n", error);
    }

    // 清理
    remove(test_file);
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("默认配置:\n");
    csv_config_t config = csv_default_config();
    printf("  分隔符: '%c'\n", config.delimiter);
    printf("  引号: '%c'\n", config.quote);
    printf("  转义: '%c'\n", config.escape);
    printf("  修剪空格: %s\n", config.trim_whitespace ? "是" : "否");
    printf("  忽略空行: %s\n", config.ignore_empty_lines ? "是" : "否");
    printf("  标题行: %s\n", config.header_row ? "是" : "否");

    printf("\n自定义配置示例:\n");
    
    const char *line = "  value1  ,  value2  ,  value3  ";
    printf("\n输入: \"%s\"\n", line);

    // 不修剪空格
    printf("\n不修剪空格:\n");
    config = csv_default_config();
    config.trim_whitespace = false;
    csv_error_t error;
    csv_row_t *row = csv_parse_line(line, &config, &error);
    if (row) {
        print_row(row);
        csv_row_free(row);
    }

    // 修剪空格
    printf("\n修剪空格:\n");
    config.trim_whitespace = true;
    row = csv_parse_line(line, &config, &error);
    if (row) {
        print_row(row);
        csv_row_free(row);
    }
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CSV_OK (%d): 成功\n", CSV_OK);
    printf("  CSV_ERROR_FILE_OPEN (%d): 文件打开失败\n", CSV_ERROR_FILE_OPEN);
    printf("  CSV_ERROR_FILE_READ (%d): 文件读取失败\n", CSV_ERROR_FILE_READ);
    printf("  CSV_ERROR_FILE_WRITE (%d): 文件写入失败\n", CSV_ERROR_FILE_WRITE);
    printf("  CSV_ERROR_PARSE (%d): 解析错误\n", CSV_ERROR_PARSE);
    printf("  CSV_ERROR_INVALID_PARAM (%d): 无效参数\n", CSV_ERROR_INVALID_PARAM);
    printf("  CSV_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CSV_ERROR_MEMORY_ALLOC);
    printf("  CSV_ERROR_COLUMN_MISMATCH (%d): 列数不匹配\n", CSV_ERROR_COLUMN_MISMATCH);
    printf("  CSV_ERROR_OUT_OF_BOUNDS (%d): 越界访问\n", CSV_ERROR_OUT_OF_BOUNDS);

    printf("\n错误场景测试:\n");
    
    // 无效参数
    csv_error_t error;
    csv_row_t *row = csv_parse_line(NULL, NULL, &error);
    printf("  NULL 输入: %s, 错误码: %d\n", row ? "成功" : "失败", error);

    // 不存在的文件
    csv_t *csv = csv_load("/nonexistent/file.csv", NULL, &error);
    printf("  不存在的文件: %s, 错误码: %d\n", csv ? "成功" : "失败", error);
}

// 演示 7: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("CSV 处理的应用场景:\n\n");

    printf("1. 数据导入/导出:\n");
    printf("   - Excel 数据交换\n");
    printf("   - 数据库导入导出\n");
    printf("   - 报表生成\n\n");

    printf("2. 配置管理:\n");
    printf("   - 参数配置表\n");
    printf("   - 映射表维护\n");
    printf("   - 多语言支持\n\n");

    printf("3. 数据分析:\n");
    printf("   - 日志分析\n");
    printf("   - 统计数据处理\n");
    printf("   - 数据清洗\n\n");

    printf("4. 系统集成:\n");
    printf("   - 跨系统数据交换\n");
    printf("   - ETL 流程\n");
    printf("   - 批处理作业\n");
}

// 演示 8: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 8: 最佳实践 ===\n");

    printf("CSV 处理最佳实践:\n\n");

    printf("1. 文件格式:\n");
    printf("   - 使用 UTF-8 编码\n");
    printf("   - 统一换行符 (LF)\n");
    printf("   - 添加 BOM 标识\n\n");

    printf("2. 数据质量:\n");
    printf("   - 验证列数一致性\n");
    printf("   - 处理缺失值\n");
    printf("   - 转义特殊字符\n\n");

    printf("3. 性能优化:\n");
    printf("   - 使用流式处理大文件\n");
    printf("   - 预分配内存\n");
    printf("   - 避免重复解析\n\n");

    printf("4. 错误处理:\n");
    printf("   - 记录解析错误\n");
    printf("   - 跳过无效行\n");
    printf("   - 提供降级方案\n");
}

int main(void) {
    printf("========================================\n");
    printf("    CSV 解析演示程序\n");
    printf("========================================\n");

    demo_basic();
    demo_delimiters();
    demo_quotes();
    demo_file_read();
    demo_config();
    demo_errors();
    demo_use_cases();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
