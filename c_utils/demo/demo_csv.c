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
#include <time.h>
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

// 演示 6.1: 边界情况测试
static void demo_edge_cases(void) {
    printf("\n=== 演示 6.1: 边界情况测试 ===\n");

    // 空行
    printf("空行测试:\n");
    const char *empty_lines[] = {
        "",                    // 完全空行
        "   ",                  // 只有空格
        "\n",                  // 只有换行符
        "\r\n"                // 回车换行
    };
    int num_empty = sizeof(empty_lines) / sizeof(empty_lines[0]);
    for (int i = 0; i < num_empty; i++) {
        printf("  输入: '%s' (长度: %zu)\n", empty_lines[i], strlen(empty_lines[i]));
        csv_error_t error;
        csv_row_t *row = csv_parse_line(empty_lines[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }

    // 特殊字符
    printf("\n特殊字符测试:\n");
    const char *special_lines[] = {
        "field1,field2\\nfield3",  // 包含转义换行符
        "field1,field2\\rfield3",  // 包含转义回车符
        "field1,field2\\tfield3",  // 包含转义制表符
        "field1,field2\\,field3"   // 包含转义逗号
    };
    int num_special = sizeof(special_lines) / sizeof(special_lines[0]);
    for (int i = 0; i < num_special; i++) {
        printf("  输入: %s\n", special_lines[i]);
        csv_error_t error;
        csv_row_t *row = csv_parse_line(special_lines[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }

    // 超长行
    printf("\n超长行测试:\n");
    char long_line[1024];
    strcpy(long_line, "field1");
    for (int i = 0; i < 100; i++) {
        strcat(long_line, ",field");
    }
    printf("  超长行长度: %zu\n", strlen(long_line));
    csv_error_t error;
    csv_row_t *row = csv_parse_line(long_line, NULL, &error);
    if (row) {
        printf("  解析成功，字段数: %zu\n", row->count);
        csv_row_free(row);
    } else {
        printf("  解析失败，错误码: %d\n", error);
    }
}

// 演示 6.2: 数据完整性测试
static void demo_data_integrity(void) {
    printf("\n=== 演示 6.2: 数据完整性测试 ===\n");

    // 列数不一致
    printf("列数不一致测试:\n");
    const char *mismatch_lines[] = {
        "name,age,city",         // 3列
        "Alice,30",              // 2列
        "Bob,25,Los Angeles,CA"  // 4列
    };
    int num_mismatch = sizeof(mismatch_lines) / sizeof(mismatch_lines[0]);
    
    csv_t *csv = csv_create(NULL, NULL);
    if (csv) {
        for (int i = 0; i < num_mismatch; i++) {
            csv_error_t error;
            csv_row_t *row = csv_parse_line(mismatch_lines[i], NULL, &error);
            if (row) {
                bool result = csv_add_row(csv, (const char **)row->fields, row->count, &error);
                printf("  行 %d (字段数: %zu): %s, 错误码: %d\n", 
                       i, row->count, result ? "成功" : "失败", error);
                csv_row_free(row);
            }
        }
        csv_free(csv);
    }

    // 空字段
    printf("\n空字段测试:\n");
    const char *empty_fields[] = {
        "field1,,field3",        // 中间空字段
        ",field2,field3",        // 开头空字段
        "field1,field2,",        // 结尾空字段
        ",,"                     // 全部空字段
    };
    int num_empty_fields = sizeof(empty_fields) / sizeof(empty_fields[0]);
    for (int i = 0; i < num_empty_fields; i++) {
        printf("  输入: %s\n", empty_fields[i]);
        csv_error_t error;
        csv_row_t *row = csv_parse_line(empty_fields[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }
}

// 演示 6.3: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 6.3: 性能测试 ===\n");

    // 批量解析测试
    const char *test_lines[] = {
        "name,age,city",
        "Alice,30,New York",
        "Bob,25,Los Angeles",
        "Charlie,35,San Francisco",
        "David,28,Boston"
    };
    int num_test_lines = sizeof(test_lines) / sizeof(test_lines[0]);
    int iterations = 10000;

    printf("批量解析测试: %d 次迭代\n", iterations);
    printf("测试行数: %d\n", num_test_lines);
    
    clock_t start = clock();
    csv_error_t error;
    
    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < num_test_lines; j++) {
            csv_row_t *row = csv_parse_line(test_lines[j], NULL, &error);
            if (row) {
                csv_row_free(row);
            }
        }
    }
    
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("执行时间: %.4f 秒\n", time_taken);
    printf("每秒解析行数: %.0f\n", (double)(iterations * num_test_lines) / time_taken);
}

// 演示 6.4: 扩展边界情况测试
static void demo_edge_cases_extended(void) {
    printf("\n=== 演示 6.4: 扩展边界情况测试 ===\n");

    // 极短行
    printf("极短行测试:\n");
    const char *short_lines[] = {
        "a",                   // 单个字符
        ",",                  // 单个逗号
        "\"\"",               // 空引号
        "a,"
    };
    int num_short = sizeof(short_lines) / sizeof(short_lines[0]);
    for (int i = 0; i < num_short; i++) {
        printf("  输入: '%s'\n", short_lines[i]);
        csv_error_t error;
        csv_row_t *row = csv_parse_line(short_lines[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }

    // 嵌套引号
    printf("\n嵌套引号测试:\n");
    const char *nested_quotes[] = {
        "\"\"\"",              // 三层引号
        "\"\"text\"\"",         // 引号内引号
        "\"a,\"b\"c\""        // 复杂嵌套
    };
    int num_nested = sizeof(nested_quotes) / sizeof(nested_quotes[0]);
    for (int i = 0; i < num_nested; i++) {
        printf("  输入: %s\n", nested_quotes[i]);
        csv_error_t error;
        csv_row_t *row = csv_parse_line(nested_quotes[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }

    // 混合内容
    printf("\n混合内容测试:\n");
    const char *mixed_content[] = {
        "123,45.67,true",      // 数字和布尔值
        "line1\nline2,line3",   // 换行符
        "\t\tvalue\t\t",       // 制表符
        "a\\b\\c,d\\e\\f"       // 反斜杠
    };
    int num_mixed = sizeof(mixed_content) / sizeof(mixed_content[0]);
    for (int i = 0; i < num_mixed; i++) {
        printf("  输入: %s\n", mixed_content[i]);
        csv_error_t error;
        csv_row_t *row = csv_parse_line(mixed_content[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }
}

// 演示 6.5: 扩展异常情况测试
static void demo_error_cases_extended(void) {
    printf("\n=== 演示 6.5: 扩展异常情况测试 ===\n");

    // 格式错误
    printf("格式错误测试:\n");
    const char *format_errors[] = {
        "\"unclosed quote",       // 未闭合的引号
        "quote\"without\"comma",  // 引号内无逗号
        "field1,field2,"           // 末尾逗号
    };
    int num_format = sizeof(format_errors) / sizeof(format_errors[0]);
    for (int i = 0; i < num_format; i++) {
        printf("  输入: %s\n", format_errors[i]);
        csv_error_t error;
        csv_row_t *row = csv_parse_line(format_errors[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }

    // 内存分配测试（模拟）
    printf("\n内存分配测试:\n");
    // 注意：这里只是测试接口，实际内存分配失败需要在特定环境下测试
    csv_error_t error;
    csv_t *csv = csv_create(NULL, &error);
    if (csv) {
        printf("  CSV 创建成功\n");
        csv_free(csv);
    } else {
        printf("  CSV 创建失败，错误码: %d\n", error);
    }

    // 越界访问测试
    printf("\n越界访问测试:\n");
    const char *test_file = "/tmp/csv_test_boundary.csv";
    FILE *fp = fopen(test_file, "w");
    if (fp) {
        fprintf(fp, "a,b,c\n");
        fprintf(fp, "1,2,3\n");
        fclose(fp);

        csv_t *csv = csv_load(test_file, NULL, &error);
        if (csv) {
            // 测试正常访问
            const char *val = csv_get(csv, 0, 0, &error);
            printf("  正常访问 (0,0): %s, 错误码: %d\n", val ? val : "失败", error);

            // 测试行越界
            val = csv_get(csv, 10, 0, &error);
            printf("  行越界 (10,0): %s, 错误码: %d\n", val ? val : "失败", error);

            // 测试列越界
            val = csv_get(csv, 0, 10, &error);
            printf("  列越界 (0,10): %s, 错误码: %d\n", val ? val : "失败", error);

            csv_free(csv);
        }
        remove(test_file);
    }
}

// 演示 6.6: 扩展性能测试
static void demo_performance_extended(void) {
    printf("\n=== 演示 6.6: 扩展性能测试 ===\n");

    // 大文件处理测试
    printf("大文件处理测试:\n");
    const char *big_file = "/tmp/csv_big_test.csv";
    FILE *fp = fopen(big_file, "w");
    if (fp) {
        // 写入1000行数据
        fprintf(fp, "name,age,city\n");
        for (int i = 0; i < 1000; i++) {
            fprintf(fp, "person%d,%d,city%d\n", i, 20 + i % 50, i % 100);
        }
        fclose(fp);

        clock_t start = clock();
        csv_error_t error;
        csv_t *csv = csv_load(big_file, NULL, &error);
        clock_t end = clock();
        double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

        if (csv) {
            printf("  读取成功，行数: %zu\n", csv_get_rows(csv));
            printf("  执行时间: %.4f 秒\n", time_taken);
            csv_free(csv);
        } else {
            printf("  读取失败，错误码: %d\n", error);
        }
        remove(big_file);
    }

    // 复杂行解析测试
    printf("\n复杂行解析测试:\n");
    const char *complex_line = "\"complex,field\",\"with\"\"quotes\"\",123,true,\"line1\nline2\"";
    int iterations = 10000;

    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        csv_error_t error;
        csv_row_t *row = csv_parse_line(complex_line, NULL, &error);
        if (row) {
            csv_row_free(row);
        }
    }
    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("  解析复杂行 %d 次\n", iterations);
    printf("  执行时间: %.4f 秒\n", time_taken);
    printf("  每秒解析次数: %.0f\n", (double)iterations / time_taken);
}

// 演示 6.7: 兼容性测试
static void demo_compatibility(void) {
    printf("\n=== 演示 6.7: 兼容性测试 ===\n");

    // 不同换行符测试
    printf("不同换行符测试:\n");
    const char *newline_tests[] = {
        "a,b,c\n",          // LF
        "a,b,c\r\n",        // CRLF
        "a,b,c\r"           // CR
    };
    const char *newline_names[] = {"LF", "CRLF", "CR"};
    int num_newline = sizeof(newline_tests) / sizeof(newline_tests[0]);
    for (int i = 0; i < num_newline; i++) {
        printf("  %s 换行: %s\n", newline_names[i], newline_tests[i]);
        csv_error_t error;
        csv_row_t *row = csv_parse_line(newline_tests[i], NULL, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }

    // 不同分隔符测试
    printf("\n不同分隔符测试:\n");
    const char *delimiter_tests[] = {
        "a;b;c",            // 分号
        "a|b|c",            // 管道
        "a\tb\tc",          // 制表符
        "a b c"              // 空格
    };
    char delimiters[] = {';', '|', '\t', ' '};
    const char *delimiter_names[] = {"分号", "管道", "制表符", "空格"};
    int num_delimiter = sizeof(delimiter_tests) / sizeof(delimiter_tests[0]);
    for (int i = 0; i < num_delimiter; i++) {
        printf("  %s 分隔: %s\n", delimiter_names[i], delimiter_tests[i]);
        csv_config_t config = csv_default_config();
        config.delimiter = delimiters[i];
        csv_error_t error;
        csv_row_t *row = csv_parse_line(delimiter_tests[i], &config, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }

    // 不同引号样式测试
    printf("\n不同引号样式测试:\n");
    const char *quote_tests[] = {
        "'a','b','c'",       // 单引号
        "\"a\",\"b\",\"c\"",     // 双引号
        "`a`,`b`,`c`"        // 反引号
    };
    char quotes[] = {'\'', '"', '`'};
    const char *quote_names[] = {"单引号", "双引号", "反引号"};
    int num_quote = sizeof(quote_tests) / sizeof(quote_tests[0]);
    for (int i = 0; i < num_quote; i++) {
        printf("  %s: %s\n", quote_names[i], quote_tests[i]);
        csv_config_t config = csv_default_config();
        config.quote = quotes[i];
        csv_error_t error;
        csv_row_t *row = csv_parse_line(quote_tests[i], &config, &error);
        if (row) {
            print_row(row);
            csv_row_free(row);
        } else {
            printf("  解析失败，错误码: %d\n", error);
        }
    }
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
    demo_edge_cases();
    demo_data_integrity();
    demo_performance();
    demo_edge_cases_extended();
    demo_error_cases_extended();
    demo_performance_extended();
    demo_compatibility();
    demo_use_cases();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
