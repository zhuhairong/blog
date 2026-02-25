/**
 * JSON 写入器演示程序
 *
 * 功能：
 * - 流式 JSON 生成
 * - 对象和数组构建
 * - 格式化输出
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/json_writer.h"

// 演示 1: 基本对象写入
static void demo_basic_object(void) {
    printf("\n=== 演示 1: 基本对象写入 ===\n");

    json_writer_t *writer = NULL;
    json_writer_config_t config = {0};
    config.pretty_print = false;

    char buffer[256];
    json_writer_error_t err = json_writer_create_buffer(&writer, sizeof(buffer), &config);
    if (err != JSON_WRITER_OK) {
        printf("创建 writer 失败\n");
        return;
    }

    printf("生成 JSON 对象:\n");

    json_writer_begin_object(writer);
    json_writer_key(writer, "name");
    json_writer_string(writer, "Alice");
    json_writer_key(writer, "age");
    json_writer_int(writer, 30);
    json_writer_key(writer, "active");
    json_writer_bool(writer, true);
    json_writer_end_object(writer);

    // 获取结果
    printf("%s\n", writer->output.buffer.buffer);

    json_writer_destroy(writer);
}

// 演示 2: 数组写入
static void demo_array(void) {
    printf("\n=== 演示 2: 数组写入 ===\n");

    json_writer_t *writer = NULL;
    json_writer_config_t config = {0};
    config.pretty_print = false;

    char buffer[256];
    json_writer_create_buffer(&writer, sizeof(buffer), &config);

    printf("生成 JSON 数组:\n");

    json_writer_begin_array(writer);
    json_writer_int(writer, 1);
    json_writer_int(writer, 2);
    json_writer_int(writer, 3);
    json_writer_int(writer, 4);
    json_writer_int(writer, 5);
    json_writer_end_array(writer);

    printf("%s\n", writer->output.buffer.buffer);

    json_writer_destroy(writer);
}

// 演示 3: 嵌套结构
static void demo_nested(void) {
    printf("\n=== 演示 3: 嵌套结构 ===\n");

    json_writer_t *writer = NULL;
    json_writer_config_t config = {0};
    config.pretty_print = true;
    config.indent_size = 2;

    char buffer[512];
    json_writer_create_buffer(&writer, sizeof(buffer), &config);

    printf("生成嵌套 JSON:\n");

    json_writer_begin_object(writer);

    json_writer_key(writer, "user");
    json_writer_begin_object(writer);
    json_writer_key(writer, "name");
    json_writer_string(writer, "Bob");
    json_writer_key(writer, "email");
    json_writer_string(writer, "bob@example.com");
    json_writer_end_object(writer);

    json_writer_key(writer, "hobbies");
    json_writer_begin_array(writer);
    json_writer_string(writer, "reading");
    json_writer_string(writer, "gaming");
    json_writer_string(writer, "coding");
    json_writer_end_array(writer);

    json_writer_end_object(writer);

    printf("%s\n", writer->output.buffer.buffer);

    json_writer_destroy(writer);
}

// 演示 4: 不同数据类型
static void demo_types(void) {
    printf("\n=== 演示 4: 数据类型 ===\n");

    json_writer_t *writer = NULL;
    json_writer_config_t config = {0};
    config.pretty_print = false;

    char buffer[512];
    json_writer_create_buffer(&writer, sizeof(buffer), &config);

    printf("各种数据类型:\n");

    json_writer_begin_object(writer);

    json_writer_key(writer, "null_value");
    json_writer_null(writer);

    json_writer_key(writer, "bool_true");
    json_writer_bool(writer, true);

    json_writer_key(writer, "bool_false");
    json_writer_bool(writer, false);

    json_writer_key(writer, "integer");
    json_writer_int(writer, 42);

    json_writer_key(writer, "float");
    json_writer_double(writer, 3.14159);

    json_writer_key(writer, "string");
    json_writer_string(writer, "Hello World");

    json_writer_end_object(writer);

    printf("%s\n", writer->output.buffer.buffer);

    json_writer_destroy(writer);
}

// 演示 5: 格式化选项
static void demo_formatting(void) {
    printf("\n=== 演示 5: 格式化选项 ===\n");

    // 紧凑格式
    printf("紧凑格式:\n");
    {
        json_writer_t *writer = NULL;
        json_writer_config_t config = {0};
        config.pretty_print = false;

        char buffer[256];
        json_writer_create_buffer(&writer, sizeof(buffer), &config);

        json_writer_begin_object(writer);
        json_writer_key(writer, "a");
        json_writer_int(writer, 1);
        json_writer_key(writer, "b");
        json_writer_int(writer, 2);
        json_writer_end_object(writer);

        printf("%s\n\n", writer->output.buffer.buffer);
        json_writer_destroy(writer);
    }

    // 美化格式
    printf("美化格式:\n");
    {
        json_writer_t *writer = NULL;
        json_writer_config_t config = {0};
        config.pretty_print = true;
        config.indent_size = 2;

        char buffer[256];
        json_writer_create_buffer(&writer, sizeof(buffer), &config);

        json_writer_begin_object(writer);
        json_writer_key(writer, "a");
        json_writer_int(writer, 1);
        json_writer_key(writer, "b");
        json_writer_int(writer, 2);
        json_writer_end_object(writer);

        printf("%s\n", writer->output.buffer.buffer);
        json_writer_destroy(writer);
    }
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. API 响应生成\n");
    printf("   - 构建 JSON 响应\n");
    printf("   - 流式输出\n");
    printf("\n");

    printf("2. 配置文件生成\n");
    printf("   - 程序化创建配置\n");
    printf("   - 格式化保存\n");
    printf("\n");

    printf("3. 日志输出\n");
    printf("   - 结构化日志\n");
    printf("   - 实时流式写入\n");
    printf("\n");

    printf("4. 数据序列化\n");
    printf("   - 对象转 JSON\n");
    printf("   - 网络传输\n");
}

// 辅助函数声明（如果头文件中没有）
extern json_writer_error_t json_writer_key(json_writer_t *jw, const char *key);
extern json_writer_error_t json_writer_string(json_writer_t *jw, const char *value);
extern json_writer_error_t json_writer_int(json_writer_t *jw, int value);
extern json_writer_error_t json_writer_double(json_writer_t *jw, double value);
extern json_writer_error_t json_writer_bool(json_writer_t *jw, bool value);
extern json_writer_error_t json_writer_null(json_writer_t *jw);

int main(void) {
    printf("========================================\n");
    printf("    JSON 写入器演示\n");
    printf("========================================\n");

    demo_basic_object();
    demo_array();
    demo_nested();
    demo_types();
    demo_formatting();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
