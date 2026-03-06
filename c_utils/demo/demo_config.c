/**
 * @file demo_config.c
 * @brief 配置文件解析演示
 *
 * 本演示展示了配置文件的解析和操作，包括：
 * - 配置文件加载和保存
 * - 键值对读写
 * - 数据类型转换
 * - INI 和 JSON 格式支持
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/config.h"

static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本配置操作 ===\n");

    config_t* cfg = config_create();
    if (!cfg) {
        fprintf(stderr, "创建配置失败\n");
        return;
    }

    config_set_string(cfg, "app", "name", "MyApplication");
    config_set_string(cfg, "app", "version", "1.0.0");
    config_set_int(cfg, "app", "port", 8080);
    config_set_bool(cfg, "app", "debug", true);
    config_set_double(cfg, "app", "timeout", 30.5);

    printf("配置值:\n");
    printf("  app.name = %s\n", config_get_string(cfg, "app", "name", "unknown"));
    printf("  app.version = %s\n", config_get_string(cfg, "app", "version", "unknown"));
    printf("  app.port = %d\n", config_get_int(cfg, "app", "port", 0));
    printf("  app.debug = %s\n", config_get_bool(cfg, "app", "debug", false) ? "true" : "false");
    printf("  app.timeout = %.2f\n", config_get_double(cfg, "app", "timeout", 0.0));

    printf("\n不存在的键:\n");
    printf("  app.missing = %s (默认值)\n",
           config_get_string(cfg, "app", "missing", "default_value"));

    config_free(cfg);
}

static void demo_data_types(void) {
    printf("\n=== 演示 2: 数据类型 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    config_set_string(cfg, NULL, "title", "My Application");
    printf("字符串: %s\n", config_get_string(cfg, NULL, "title", ""));

    config_set_int(cfg, NULL, "count", 42);
    printf("整数: %d\n", config_get_int(cfg, NULL, "count", 0));

    config_set_double(cfg, NULL, "pi", 3.14159);
    printf("浮点数: %.5f\n", config_get_double(cfg, NULL, "pi", 0.0));

    config_set_bool(cfg, NULL, "enabled", true);
    printf("布尔值: %s\n", config_get_bool(cfg, NULL, "enabled", false) ? "true" : "false");

    printf("\n修改后的值:\n");
    config_set_int(cfg, NULL, "count", 100);
    printf("count = %d\n", config_get_int(cfg, NULL, "count", 0));

    config_free(cfg);
}

static void demo_sections(void) {
    printf("\n=== 演示 3: 多节配置 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    config_set_string(cfg, "database", "host", "localhost");
    config_set_int(cfg, "database", "port", 3306);
    config_set_string(cfg, "database", "name", "mydb");
    config_set_string(cfg, "database", "user", "admin");

    config_set_string(cfg, "log", "level", "info");
    config_set_string(cfg, "log", "file", "/var/log/app.log");
    config_set_bool(cfg, "log", "console", true);

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

static void demo_file_operations(void) {
    printf("\n=== 演示 4: 配置文件加载和保存 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    config_set_string(cfg, "server", "host", "0.0.0.0");
    config_set_int(cfg, "server", "port", 8080);
    config_set_string(cfg, "database", "url", "postgres://localhost/mydb");
    config_set_int(cfg, "database", "pool_size", 10);

    const char* filename = "/tmp/demo_config.ini";
    config_error_t error;
    
    printf("保存配置到 %s...\n", filename);
    if (config_save(cfg, filename, CONFIG_FORMAT_INI, &error)) {
        printf("  保存成功\n");
    } else {
        printf("  保存失败: %s\n", config_strerror(error));
    }

    config_free(cfg);

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
        printf("  加载失败: %s\n", config_strerror(error));
    }
}

static void demo_json_format(void) {
    printf("\n=== 演示 5: JSON 格式支持 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    config_set_string(cfg, "app", "name", "JSON Demo");
    config_set_string(cfg, "app", "version", "2.0.0");
    config_set_int(cfg, "app", "port", 9000);
    config_set_bool(cfg, "app", "debug", false);
    
    config_set_string(cfg, "", "title", "Global Setting");
    config_set_int(cfg, "", "max_connections", 100);
    config_set_bool(cfg, "", "enabled", true);

    const char* filename = "/tmp/demo_config.json";
    config_error_t error;
    
    printf("保存 JSON 配置到 %s...\n", filename);
    if (config_save(cfg, filename, CONFIG_FORMAT_JSON, &error)) {
        printf("  保存成功\n");
        
        printf("\nJSON 文件内容:\n");
        FILE* fp = fopen(filename, "r");
        if (fp) {
            char line[256];
            while (fgets(line, sizeof(line), fp)) {
                printf("  %s", line);
            }
            fclose(fp);
            printf("\n");
        }
    } else {
        printf("  保存失败: %s\n", config_strerror(error));
    }

    config_free(cfg);

    printf("\n从 JSON 文件加载配置...\n");
    cfg = config_load(filename, CONFIG_FORMAT_JSON, &error);
    
    if (cfg) {
        printf("  加载成功\n");
        printf("  app.name = %s\n", config_get_string(cfg, "app", "name", ""));
        printf("  app.version = %s\n", config_get_string(cfg, "app", "version", ""));
        printf("  app.port = %d\n", config_get_int(cfg, "app", "port", 0));
        printf("  title = %s\n", config_get_string(cfg, "", "title", ""));
        printf("  max_connections = %d\n", config_get_int(cfg, "", "max_connections", 0));
        printf("  enabled = %s\n", config_get_bool(cfg, "", "enabled", false) ? "true" : "false");
        config_free(cfg);
    } else {
        printf("  加载失败: %s\n", config_strerror(error));
    }
}

static void demo_auto_format(void) {
    printf("\n=== 演示 6: 自动格式检测 ===\n");

    printf("创建测试文件...\n");
    
    config_t* cfg = config_create();
    if (!cfg) return;
    
    config_set_string(cfg, "test", "key", "value");
    config_set_int(cfg, "test", "number", 42);
    
    config_error_t error;
    config_save(cfg, "/tmp/auto_test.ini", CONFIG_FORMAT_INI, &error);
    config_save(cfg, "/tmp/auto_test.json", CONFIG_FORMAT_JSON, &error);
    config_free(cfg);
    
    printf("\n自动检测 INI 文件:\n");
    cfg = config_load("/tmp/auto_test.ini", CONFIG_FORMAT_AUTO, &error);
    if (cfg) {
        printf("  成功加载 INI: test.key = %s\n", config_get_string(cfg, "test", "key", ""));
        config_free(cfg);
    }
    
    printf("\n自动检测 JSON 文件:\n");
    cfg = config_load("/tmp/auto_test.json", CONFIG_FORMAT_AUTO, &error);
    if (cfg) {
        printf("  成功加载 JSON: test.key = %s\n", config_get_string(cfg, "test", "key", ""));
        config_free(cfg);
    }
}

static void demo_error_handling(void) {
    printf("\n=== 演示 7: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CONFIG_OK (%d): %s\n", CONFIG_OK, config_strerror(CONFIG_OK));
    printf("  CONFIG_ERROR_FILE_OPEN (%d): %s\n", CONFIG_ERROR_FILE_OPEN, config_strerror(CONFIG_ERROR_FILE_OPEN));
    printf("  CONFIG_ERROR_FILE_READ (%d): %s\n", CONFIG_ERROR_FILE_READ, config_strerror(CONFIG_ERROR_FILE_READ));
    printf("  CONFIG_ERROR_FILE_WRITE (%d): %s\n", CONFIG_ERROR_FILE_WRITE, config_strerror(CONFIG_ERROR_FILE_WRITE));
    printf("  CONFIG_ERROR_PARSE (%d): %s\n", CONFIG_ERROR_PARSE, config_strerror(CONFIG_ERROR_PARSE));
    printf("  CONFIG_ERROR_MEMORY_ALLOC (%d): %s\n", CONFIG_ERROR_MEMORY_ALLOC, config_strerror(CONFIG_ERROR_MEMORY_ALLOC));
    printf("  CONFIG_ERROR_UNSUPPORTED_FORMAT (%d): %s\n", CONFIG_ERROR_UNSUPPORTED_FORMAT, config_strerror(CONFIG_ERROR_UNSUPPORTED_FORMAT));

    printf("\n测试加载不存在的文件:\n");
    config_error_t error;
    config_t* cfg = config_load("/nonexistent/file.ini", CONFIG_FORMAT_INI, &error);
    if (!cfg) {
        printf("  预期中的失败，错误: %s\n", config_strerror(error));
    }
    
    printf("\n测试 YAML 格式 (未实现):\n");
    cfg = config_load("/tmp/test.yaml", CONFIG_FORMAT_YAML, &error);
    if (!cfg) {
        printf("  预期中的失败，错误: %s\n", config_strerror(error));
    }
}

static void demo_sections_keys(void) {
    printf("\n=== 演示 8: 节和键操作 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    config_set_string(cfg, "database", "host", "localhost");
    config_set_string(cfg, "database", "port", "3306");
    config_set_string(cfg, "database", "name", "mydb");
    
    config_set_string(cfg, "server", "host", "0.0.0.0");
    config_set_string(cfg, "server", "port", "8080");
    
    config_set_string(cfg, "", "global_key", "global_value");

    size_t section_count;
    char** sections = config_get_sections(cfg, &section_count);
    
    printf("所有节 (%zu 个):\n", section_count);
    for (size_t i = 0; i < section_count; i++) {
        printf("  [%s]\n", sections[i]);
        
        size_t key_count;
        char** keys = config_get_keys(cfg, sections[i], &key_count);
        for (size_t j = 0; j < key_count; j++) {
            const char* val = config_get_string(cfg, sections[i], keys[j], "");
            printf("    %s = %s\n", keys[j], val);
        }
        config_free_keys(keys, key_count);
    }
    config_free_sections(sections, section_count);

    config_free(cfg);
}

static void demo_delete_clear(void) {
    printf("\n=== 演示 9: 删除和清除操作 ===\n");

    config_t* cfg = config_create();
    if (!cfg) return;

    config_set_string(cfg, "sec", "key1", "value1");
    config_set_string(cfg, "sec", "key2", "value2");
    config_set_string(cfg, "sec", "key3", "value3");
    
    printf("初始配置项数量: %zu\n", config_count(cfg));
    
    printf("\n删除 key1...\n");
    if (config_delete(cfg, "sec", "key1")) {
        printf("  删除成功\n");
        printf("  当前配置项数量: %zu\n", config_count(cfg));
    }
    
    printf("\n检查键是否存在:\n");
    printf("  key1 存在: %s\n", config_exists(cfg, "sec", "key1") ? "是" : "否");
    printf("  key2 存在: %s\n", config_exists(cfg, "sec", "key2") ? "是" : "否");
    
    printf("\n清除所有配置...\n");
    config_clear(cfg);
    printf("  清除后配置项数量: %zu\n", config_count(cfg));

    config_free(cfg);
}

static void demo_performance(void) {
    printf("\n=== 演示 10: 性能测试 ===\n");

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

static void demo_best_practices(void) {
    printf("\n=== 演示 11: 最佳实践 ===\n");

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
    printf("   - 示例配置\n\n");
    
    printf("5. 格式选择:\n");
    printf("   - INI: 简单配置，易于手动编辑\n");
    printf("   - JSON: 结构化配置，支持嵌套\n");
    printf("   - 使用 CONFIG_FORMAT_AUTO 自动检测\n");
}

int main(void) {
    printf("========================================\n");
    printf("    配置文件解析演示程序\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_data_types();
    demo_sections();
    demo_file_operations();
    demo_json_format();
    demo_auto_format();
    demo_error_handling();
    demo_sections_keys();
    demo_delete_clear();
    demo_performance();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
