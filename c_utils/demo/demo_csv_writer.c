/**
 * @file demo_csv_writer.c
 * @brief CSV 写入器演示
 *
 * 本演示展示了 CSV 文件的写入操作，包括：
 * - 基本 CSV 写入
 * - 格式化选项
 * - 缓冲区管理
 * - 错误处理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/csv_writer.h"

// 演示 1: 基本 CSV 写入
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本 CSV 写入 ===\n");

    const char *test_file = "/tmp/csv_write_test.csv";
    
    printf("创建 CSV 文件: %s\n\n", test_file);

    csv_writer_error_t error;
    csv_writer_t *writer = csv_writer_create(test_file, "w", NULL, &error);
    if (!writer) {
        printf("创建写入器失败，错误码: %d\n", error);
        return;
    }

    // 写入标题行
    printf("写入标题行...\n");
    csv_writer_write_field(writer, "Name", false);
    csv_writer_write_field(writer, "Age", false);
    csv_writer_write_field(writer, "City", true);

    // 写入数据行
    printf("写入数据行...\n");
    csv_writer_write_field(writer, "Alice", false);
    csv_writer_write_field(writer, "30", false);
    csv_writer_write_field(writer, "New York", true);

    csv_writer_write_field(writer, "Bob", false);
    csv_writer_write_field(writer, "25", false);
    csv_writer_write_field(writer, "Los Angeles", true);

    csv_writer_write_field(writer, "Carol", false);
    csv_writer_write_field(writer, "35", false);
    csv_writer_write_field(writer, "Chicago", true);

    csv_writer_free(writer);

    // 读取并显示文件内容
    printf("\n文件内容:\n");
    FILE *fp = fopen(test_file, "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            printf("  %s", line);
        }
        fclose(fp);
    }

    remove(test_file);
}

// 演示 2: 特殊字符处理
static void demo_special_chars(void) {
    printf("\n=== 演示 2: 特殊字符处理 ===\n");

    const char *test_file = "/tmp/csv_special.csv";
    csv_writer_error_t error;
    csv_writer_t *writer = csv_writer_create(test_file, "w", NULL, &error);
    if (!writer) {
        printf("创建写入器失败\n");
        return;
    }

    printf("包含特殊字符的数据:\n\n");

    // 包含逗号
    printf("1. 包含逗号:\n");
    csv_writer_write_field(writer, "Smith, John", true);

    // 包含引号
    printf("2. 包含引号:\n");
    csv_writer_write_field(writer, "He said \"Hello\"", true);

    // 包含换行
    printf("3. 包含换行:\n");
    csv_writer_write_field(writer, "Line 1\nLine 2", true);

    // 包含空格
    printf("4. 包含首尾空格:\n");
    csv_writer_write_field(writer, "  spaced text  ", true);

    csv_writer_free(writer);

    // 读取并显示
    printf("\n文件内容:\n");
    FILE *fp = fopen(test_file, "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            printf("  %s", line);
        }
        fclose(fp);
    }

    remove(test_file);
}

// 演示 3: 整行写入
static void demo_write_row(void) {
    printf("\n=== 演示 3: 整行写入 ===\n");

    const char *test_file = "/tmp/csv_row_test.csv";
    csv_writer_error_t error;
    csv_writer_t *writer = csv_writer_create(test_file, "w", NULL, &error);
    if (!writer) {
        printf("创建写入器失败\n");
        return;
    }

    printf("使用 csv_writer_write_row 写入多行:\n\n");

    // 使用字段数组写入
    const char *row1[] = {"Product", "Price", "Quantity"};
    const char *row2[] = {"Apple", "1.50", "100"};
    const char *row3[] = {"Banana", "0.75", "200"};
    const char *row4[] = {"Orange", "2.00", "150"};

    csv_writer_write_row(writer, row1, 3);
    csv_writer_write_row(writer, row2, 3);
    csv_writer_write_row(writer, row3, 3);
    csv_writer_write_row(writer, row4, 3);

    csv_writer_free(writer);

    printf("文件内容:\n");
    FILE *fp = fopen(test_file, "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            printf("  %s", line);
        }
        fclose(fp);
    }

    remove(test_file);
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    printf("默认配置:\n");
    csv_writer_config_t config = csv_writer_default_config();
    printf("  分隔符: '%c'\n", config.delimiter);
    printf("  引号: '%c'\n", config.quote);
    printf("  转义: '%c'\n", config.escape);
    printf("  引用所有字段: %s\n", config.quote_all_fields ? "是" : "否");
    printf("  修剪空格: %s\n", config.trim_whitespace ? "是" : "否");
    printf("  缓冲区大小: %zu\n", config.buffer_size);

    printf("\n自定义分隔符 (分号):\n");
    config = csv_writer_default_config();
    config.delimiter = ';';
    
    const char *test_file = "/tmp/csv_semicolon.csv";
    csv_writer_error_t error;
    csv_writer_t *writer = csv_writer_create(test_file, "w", &config, &error);
    if (writer) {
        csv_writer_write_field(writer, "A", false);
        csv_writer_write_field(writer, "B", false);
        csv_writer_write_field(writer, "C", true);
        csv_writer_free(writer);

        FILE *fp = fopen(test_file, "r");
        if (fp) {
            char line[256];
            if (fgets(line, sizeof(line), fp)) {
                printf("  %s", line);
            }
            fclose(fp);
        }
        remove(test_file);
    }
}

// 演示 5: 追加模式
static void demo_append(void) {
    printf("\n=== 演示 5: 追加模式 ===\n");

    const char *test_file = "/tmp/csv_append.csv";
    csv_writer_error_t error;

    // 第一次写入
    printf("第一次写入:\n");
    csv_writer_t *writer = csv_writer_create(test_file, "w", NULL, &error);
    if (writer) {
        csv_writer_write_field(writer, "Header1", false);
        csv_writer_write_field(writer, "Header2", true);
        csv_writer_free(writer);
    }

    // 追加写入
    printf("追加写入:\n");
    writer = csv_writer_create(test_file, "a", NULL, &error);
    if (writer) {
        csv_writer_write_field(writer, "Data1", false);
        csv_writer_write_field(writer, "Data2", true);
        csv_writer_free(writer);
    }

    printf("\n最终文件内容:\n");
    FILE *fp = fopen(test_file, "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            printf("  %s", line);
        }
        fclose(fp);
    }

    remove(test_file);
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CSV_WRITER_OK (%d): 成功\n", CSV_WRITER_OK);
    printf("  CSV_WRITER_ERROR_FILE_OPEN (%d): 文件打开失败\n", CSV_WRITER_ERROR_FILE_OPEN);
    printf("  CSV_WRITER_ERROR_FILE_WRITE (%d): 文件写入失败\n", CSV_WRITER_ERROR_FILE_WRITE);
    printf("  CSV_WRITER_ERROR_INVALID_PARAM (%d): 无效参数\n", CSV_WRITER_ERROR_INVALID_PARAM);
    printf("  CSV_WRITER_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CSV_WRITER_ERROR_MEMORY_ALLOC);
    printf("  CSV_WRITER_ERROR_BUFFER_FULL (%d): 缓冲区满\n", CSV_WRITER_ERROR_BUFFER_FULL);

    printf("\n错误场景测试:\n");
    
    // 无效路径
    csv_writer_error_t error;
    csv_writer_t *writer = csv_writer_create("/invalid/path/file.csv", "w", NULL, &error);
    printf("  无效路径: %s, 错误码: %d\n", writer ? "成功" : "失败", error);
    if (writer) csv_writer_free(writer);

    // NULL 参数
    writer = csv_writer_create(NULL, "w", NULL, &error);
    printf("  NULL 路径: %s, 错误码: %d\n", writer ? "成功" : "失败", error);
}

// 演示 7: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 7: 性能测试 ===\n");

    const char *test_file = "/tmp/csv_perf.csv";
    const int num_rows = 10000;
    const int num_cols = 5;

    csv_writer_error_t error;
    csv_writer_t *writer = csv_writer_create(test_file, "w", NULL, &error);
    if (!writer) {
        printf("创建写入器失败\n");
        return;
    }

    // 写入标题
    const char *headers[] = {"ID", "Name", "Value1", "Value2", "Value3"};
    csv_writer_write_row(writer, headers, num_cols);

    // 写入大量数据
    char value[32];
    for (int i = 0; i < num_rows; i++) {
        snprintf(value, sizeof(value), "%d", i);
        csv_writer_write_field(writer, value, false);
        csv_writer_write_field(writer, "TestName", false);
        csv_writer_write_field(writer, "100.50", false);
        csv_writer_write_field(writer, "200.75", false);
        csv_writer_write_field(writer, "300.25", true);
    }

    csv_writer_free(writer);

    // 检查文件大小
    FILE *fp = fopen(test_file, "r");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fclose(fp);
        
        printf("写入 %d 行 x %d 列数据\n", num_rows, num_cols);
        printf("文件大小: %.2f KB\n", size / 1024.0);
        printf("平均每行: %.2f 字节\n", (double)size / num_rows);
    }

    remove(test_file);
}

// 演示 8: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 8: 应用场景 ===\n");

    printf("CSV 写入的应用场景:\n\n");

    printf("1. 数据导出:\n");
    printf("   - 数据库查询结果导出\n");
    printf("   - 报表生成\n");
    printf("   - 日志导出\n\n");

    printf("2. 数据交换:\n");
    printf("   - 系统间数据传递\n");
    printf("   - API 响应格式\n");
    printf("   - 批量数据导入\n\n");

    printf("3. 配置管理:\n");
    printf("   - 参数配置表\n");
    printf("   - 映射关系表\n");
    printf("   - 多语言资源\n\n");

    printf("4. 数据分析:\n");
    printf("   - 统计结果输出\n");
    printf("   - 实验数据记录\n");
    printf("   - 测试结果导出\n");
}

// 演示 9: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 9: 最佳实践 ===\n");

    printf("CSV 写入最佳实践:\n\n");

    printf("1. 数据准备:\n");
    printf("   - 验证数据格式\n");
    printf("   - 处理特殊字符\n");
    printf("   - 统一编码 (UTF-8)\n\n");

    printf("2. 性能优化:\n");
    printf("   - 使用缓冲区\n");
    printf("   - 批量写入\n");
    printf("   - 避免频繁刷新\n\n");

    printf("3. 错误处理:\n");
    printf("   - 检查写入结果\n");
    printf("   - 记录错误日志\n");
    printf("   - 提供回滚机制\n\n");

    printf("4. 文件管理:\n");
    printf("   - 使用临时文件\n");
    printf("   - 原子性写入\n");
    printf("   - 定期清理旧文件\n");
}

int main(void) {
    printf("========================================\n");
    printf("    CSV 写入器演示程序\n");
    printf("========================================\n");

    demo_basic();
    demo_special_chars();
    demo_write_row();
    demo_config();
    demo_append();
    demo_errors();
    demo_performance();
    demo_use_cases();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
