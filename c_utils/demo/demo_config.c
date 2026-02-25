/**
 * @file demo_config.c
 * @brief 配置文件解析演示
 *
 * 本演示展示了配置文件的解析和操作，包括：
 * - 配置文件加载和保存
 * - 键值对读写
 * - 数据类型转换
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/config.h"

// 演示 1: 基本配置操作
static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本配置操作 ===\n");

    config_t* cfg = config_create();
    if (!cfg) {
        fprintf(stderr, "创建配置失败\n");
        return;
    }

    // 设置配置值
    config_set_string(cfg, "app", "name", "MyApplication");
    config_set_string(cfg, "app", "version", "1.0.0");
    config_set_int(cfg, "app", "port", 8080);
    config_set_bool(cfg, "app", "debug", true);
    config_set_double(cfg, "app", "timeout", 30.5);

    // 读取配置值
    printf("配置值:\n");
    printf("  app.name = %s\n", config_get_string(cfg, "app", "name", "unknown"));
    printf("  app.version = %s\n", config_get_string(cfg, "app", "version", "unknown"));
    printf("  app.port = %d\n", config_get_int(cfg, "app", "port", 0));
    printf("  app.debug = %s\n", config_get_bool(cfg, "app", "debug", false) ? "true" : "false");
    printf("  app.timeout = %.2f\n", config_get_double(cfg, "app", "timeout", 0.0));

    // 不存在的键
    printf("\n不存在的键:\n");
    printf("  app.missing = %s (默认值)\n",
           config_get_string(cfg, "app", "missing", "default_value"));

    config_free(cfg);
}

// 演示 2: 数据类型
static void demo_data_types(void) {
    printf("\n=== 演示 2: 数据类型 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    // 字符串
    config_set_string(cfg, NULL, "title", "My Application");
    printf("字符串: %s\n", config_get_string(cfg, NULL, "title", ""));

    // 整数
    config_set_int(cfg, NULL, "count", 42);
    printf("整数: %d\n", config_get_int(cfg, NULL, "count", 0));

    // 浮点数
    config_set_double(cfg, NULL, "pi", 3.14159);
    printf("浮点数: %.5f\n", config_get_double(cfg, NULL, "pi", 0.0));

    // 布尔值
    config_set_bool(cfg, NULL, "enabled", true);
    printf("布尔值: %s\n", config_get_bool(cfg, NULL, "enabled", false) ? "true" : "false");

    // 修改值
    printf("\n修改后的值:\n");
    config_set_int(cfg, NULL, "count", 100);
    printf("count = %d\n", config_get_int(cfg, NULL, "count", 0));

    config_free(cfg);
}

// 演示 3: 多节配置
static void demo_sections(void) {
    printf("\n=== 演示 3: 多节配置 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    // 数据库配置
    config_set_string(cfg, "database", "host", "localhost");
    config_set_int(cfg, "database", "port", 3306);
    config_set_string(cfg, "database", "name", "mydb");
    config_set_string(cfg, "database", "user", "admin");

    // 日志配置
    config_set_string(cfg, "log", "level", "info");
    config_set_string(cfg, "log", "file", "/var/log/app.log");
    config_set_bool(cfg, "log", "console", true);

    // 缓存配置
    config_set_string(cfg, "cache", "type", "redis");
    config_set_int(cfg, "cache", "ttl", 3600);
    config_set_int(cfg, "cache", "size", 1000);

    printf("数据库配置:\n");
    printf("  host = %s\n", config_get_string(cfg, "database", "host", ""));
    printf("  port = %d\n", config_get_int(cfg, "database", "port", 0));
    printf("  name = %s\n", config_get_string(cfg, "database", "name", ""));

    printf("\n日志配置:\n");
    printf("  level = %s\n", config_get_string(cfg, "log", "level", ""));
    printf("  file = %s\n", config_get_string(cfg, "log", "file", ""));
    printf("  console = %s\n", config_get_bool(cfg, "log", "console", false) ? "true" : "false");

    printf("\n缓存配置:\n");
    printf("  type = %s\n", config_get_string(cfg, "cache", "type", ""));
    printf("  ttl = %d\n", config_get_int(cfg, "cache", "ttl", 0));
    printf("  size = %d\n", config_get_int(cfg, "cache", "size", 0));

    config_free(cfg);
}

// 演示 4: 配置文件加载和保存
static void demo_file_operations(void) {
    printf("\n=== 演示 4: 配置文件加载和保存 ===\n");

    // 创建配置
    config_t* cfg = config_create();
    if (!cfg) return;

    config_set_string(cfg, "server", "host", "0.0.0.0");
    config_set_int(cfg, "server", "port", 8080);
    config_set_string(cfg, "database", "url", "postgres://localhost/mydb");
    config_set_int(cfg, "database", "pool_size", 10);

    // 保存到文件
    const char* filename = "/tmp/demo_config.ini";
    config_error_t error;
    
    printf("保存配置到 %s...\n", filename);
    if (config_save(cfg, filename, CONFIG_FORMAT_INI, &error)) {
        printf("  保存成功\n");
    } else {
        printf("  保存失败: 错误码 %d\n", error);
    }

    config_free(cfg);

    // 从文件加载
    printf("\n从文件加载配置...\n");
    cfg = config_load(filename, CONFIG_FORMAT_INI, &error);
    
    if (cfg) {
        printf("  加载成功\n");
        printf("  server.host = %s\n", config_get_string(cfg, "server", "host", ""));
        printf("  server.port = %d\n", config_get_int(cfg, "server", "port", 0));
        printf("  database.url = %s\n", config_get_string(cfg, "database", "url", ""));
        printf("  database.pool_size = %d\n", config_get_int(cfg, "database", "pool_size", 0));
        config_free(cfg);
    } else {
        printf("  加载失败: 错误码 %d\n", error);
    }
}

// 演示 5: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CONFIG_OK (%d): 成功\n", CONFIG_OK);
    printf("  CONFIG_ERROR_FILE_OPEN (%d): 文件打开失败\n", CONFIG_ERROR_FILE_OPEN);
    printf("  CONFIG_ERROR_FILE_READ (%d): 文件读取失败\n", CONFIG_ERROR_FILE_READ);
    printf("  CONFIG_ERROR_FILE_WRITE (%d): 文件写入失败\n", CONFIG_ERROR_FILE_WRITE);
    printf("  CONFIG_ERROR_PARSE (%d): 解析错误\n", CONFIG_ERROR_PARSE);
    printf("  CONFIG_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CONFIG_ERROR_MEMORY_ALLOC);

    // 测试加载不存在的文件
    printf("\n测试加载不存在的文件:\n");
    config_error_t error;
    config_t* cfg = config_load("/nonexistent/file.ini", CONFIG_FORMAT_INI, &error);
    if (!cfg) {
        printf("  预期中的失败，错误码: %d\n", error);
    }
}

// 演示 6: 配置文件格式
static void demo_formats(void) {
    printf("\n=== 演示 6: 配置文件格式 ===\n");

    printf("支持的格式:\n");
    printf("  CONFIG_FORMAT_INI (0): INI 格式\n");
    printf("  CONFIG_FORMAT_JSON (1): JSON 格式\n");
    printf("  CONFIG_FORMAT_YAML (2): YAML 格式\n");
    printf("  CONFIG_FORMAT_AUTO (3): 自动检测\n");

    printf("\nINI 格式示例:\n");
    printf("  [database]\n");
    printf("  host = localhost\n");
    printf("  port = 3306\n");
    printf("  name = mydb\n\n");
    printf("  [log]\n");
    printf("  level = info\n");
    printf("  file = app.log\n");
}

// 演示 7: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    // 模拟应用配置
    config_set_string(cfg, "server", "host", "0.0.0.0");
    config_set_int(cfg, "server", "port", 8080);
    config_set_int(cfg, "server", "workers", 4);

    config_set_string(cfg, "database", "url", "postgres://localhost/mydb");
    config_set_int(cfg, "database", "pool_size", 10);
    config_set_double(cfg, "database", "timeout", 30.0);

    config_set_string(cfg, "security", "jwt_secret", "my-secret-key");
    config_set_int(cfg, "security", "jwt_expiry", 3600);
    config_set_bool(cfg, "security", "ssl_enabled", true);

    printf("应用配置:\n\n");

    printf("服务器:\n");
    printf("  Host: %s\n", config_get_string(cfg, "server", "host", ""));
    printf("  Port: %d\n", config_get_int(cfg, "server", "port", 0));
    printf("  Workers: %d\n", config_get_int(cfg, "server", "workers", 0));

    printf("\n数据库:\n");
    printf("  URL: %s\n", config_get_string(cfg, "database", "url", ""));
    printf("  Pool Size: %d\n", config_get_int(cfg, "database", "pool_size", 0));
    printf("  Timeout: %.1f秒\n", config_get_double(cfg, "database", "timeout", 0.0));

    printf("\n安全:\n");
    printf("  JWT Secret: %s\n", config_get_string(cfg, "security", "jwt_secret", ""));
    printf("  JWT Expiry: %d秒\n", config_get_int(cfg, "security", "jwt_expiry", 0));
    printf("  SSL Enabled: %s\n", config_get_bool(cfg, "security", "ssl_enabled", false) ? "是" : "否");

    config_free(cfg);
}

// 演示 8: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 8: 性能测试 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    const int n = 1000;

    printf("插入 %d 个配置项...\n", n);

    clock_t start = clock();

    for (int i = 0; i < n; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        config_set_int(cfg, "performance", key, i);
    }

    clock_t end = clock();
    double insert_time = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    printf("  插入时间: %.2f ms\n", insert_time);
    printf("  平均每次: %.4f ms\n", insert_time / n);

    printf("\n查找 %d 个配置项...\n", n);

    start = clock();

    int sum = 0;
    for (int i = 0; i < n; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        sum += config_get_int(cfg, "performance", key, 0);
    }

    end = clock();
    double lookup_time = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    printf("  查找时间: %.2f ms\n", lookup_time);
    printf("  平均每次: %.4f ms\n", lookup_time / n);
    printf("  校验和: %d\n", sum);

    config_free(cfg);
}

// 演示 9: 配置继承和覆盖
static void demo_inheritance(void) {
    printf("\n=== 演示 9: 配置继承和覆盖 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    // 默认配置
    config_set_string(cfg, "default", "theme", "light");
    config_set_int(cfg, "default", "font_size", 14);
    config_set_string(cfg, "default", "language", "en");

    // 用户配置（覆盖默认值）
    config_set_string(cfg, "user", "theme", "dark");
    config_set_int(cfg, "user", "font_size", 16);

    printf("默认配置:\n");
    printf("  theme = %s\n", config_get_string(cfg, "default", "theme", ""));
    printf("  font_size = %d\n", config_get_int(cfg, "default", "font_size", 0));
    printf("  language = %s\n", config_get_string(cfg, "default", "language", ""));

    printf("\n用户配置:\n");
    printf("  theme = %s\n", config_get_string(cfg, "user", "theme", ""));
    printf("  font_size = %d\n", config_get_int(cfg, "user", "font_size", 0));
    printf("  language = %s (继承默认值)\n", config_get_string(cfg, "default", "language", ""));

    config_free(cfg);
}

// 演示 10: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 10: 最佳实践 ===\n");

    printf("配置管理最佳实践:\n\n");

    printf("1. 分层配置:\n");
    printf("   - 默认配置\n");
    printf("   - 环境配置 (dev/staging/prod)\n");
    printf("   - 用户配置\n");
    printf("   - 命令行参数\n\n");

    printf("2. 敏感信息:\n");
    printf("   - 密码使用环境变量\n");
    printf("   - 密钥使用密钥管理服务\n");
    printf("   - 不要提交敏感信息到版本控制\n\n");

    printf("3. 验证和默认值:\n");
    printf("   - 提供合理的默认值\n");
    printf("   - 启动时验证配置\n");
    printf("   - 清晰的错误信息\n\n");

    printf("4. 文档:\n");
    printf("   - 每个配置项的用途\n");
    printf("   - 有效值范围\n");
    printf("   - 示例配置\n");
}

int main(void) {
    printf("========================================\n");
    printf("    配置文件解析演示程序\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_data_types();
    demo_sections();
    demo_file_operations();
    demo_error_handling();
    demo_formats();
    demo_use_cases();
    demo_performance();
    demo_inheritance();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
