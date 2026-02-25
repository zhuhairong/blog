/**
 * INI 配置文件演示程序
 *
 * 功能：
 * - 加载和保存 INI 文件
 * - 读取和写入配置项
 * - 遍历配置
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/ini.h"

// 演示 1: 创建和基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    printf("创建 INI 对象...\n");
    ini_t *ini = ini_create();
    if (!ini) {
        printf("创建失败\n");
        return;
    }

    printf("\n设置配置项:\n");
    ini_set(ini, "database", "host", "localhost");
    ini_set(ini, "database", "port", "3306");
    ini_set(ini, "database", "name", "mydb");

    ini_set_int(ini, "server", "port", 8080);
    ini_set(ini, "server", "host", "0.0.0.0");
    ini_set_bool(ini, "server", "debug", true);

    printf("  [database]\n");
    printf("    host = %s\n", ini_get(ini, "database", "host"));
    printf("    port = %s\n", ini_get(ini, "database", "port"));
    printf("    name = %s\n", ini_get(ini, "database", "name"));

    printf("\n  [server]\n");
    printf("    port = %d\n", ini_get_int(ini, "server", "port", 0));
    printf("    host = %s\n", ini_get(ini, "server", "host"));
    printf("    debug = %s\n", ini_get_bool(ini, "server", "debug", false) ? "true" : "false");

    ini_free(ini);
}

// 演示 2: 数据类型
static void demo_types(void) {
    printf("\n=== 演示 2: 数据类型支持 ===\n");

    ini_t *ini = ini_create();

    printf("支持的数据类型:\n\n");

    // 字符串
    ini_set(ini, "types", "string", "Hello World");
    printf("  字符串: %s\n", ini_get(ini, "types", "string"));

    // 整数
    ini_set_int(ini, "types", "integer", 42);
    printf("  整数: %d\n", ini_get_int(ini, "types", "integer", 0));

    // 浮点数
    ini_set_double(ini, "types", "float", 3.14159);
    printf("  浮点数: %.5f\n", ini_get_double(ini, "types", "float", 0.0));

    // 布尔值
    ini_set_bool(ini, "types", "bool_true", true);
    ini_set_bool(ini, "types", "bool_false", false);
    printf("  布尔值: true=%s, false=%s\n",
           ini_get_bool(ini, "types", "bool_true", false) ? "true" : "false",
           ini_get_bool(ini, "types", "bool_false", true) ? "true" : "false");

    // 默认值
    printf("\n  默认值测试:\n");
    printf("    不存在的键: %d (默认 100)\n",
           ini_get_int(ini, "types", "nonexistent", 100));

    ini_free(ini);
}

// 演示 3: 配置遍历
static void visit_callback(const char *section, const char *key, const char *value, void *user_data) {
    (void)user_data;
    printf("  [%s] %s = %s\n", section, key, value);
}

static void demo_iteration(void) {
    printf("\n=== 演示 3: 配置遍历 ===\n");

    ini_t *ini = ini_create();

    // 添加一些配置
    ini_set(ini, "app", "name", "MyApp");
    ini_set(ini, "app", "version", "1.0.0");
    ini_set(ini, "database", "host", "localhost");
    ini_set(ini, "database", "port", "5432");
    ini_set(ini, "cache", "enabled", "true");
    ini_set(ini, "cache", "ttl", "3600");

    printf("遍历所有配置项:\n");
    ini_foreach(ini, visit_callback, NULL);

    printf("\n遍历特定 section (database):\n");
    ini_foreach_section(ini, "database", visit_callback, NULL);

    ini_free(ini);
}

// 演示 4: 配置管理
static void demo_management(void) {
    printf("\n=== 演示 4: 配置管理 ===\n");

    ini_t *ini = ini_create();

    ini_set(ini, "section1", "key1", "value1");
    ini_set(ini, "section1", "key2", "value2");
    ini_set(ini, "section2", "key1", "value3");

    printf("初始配置:\n");
    printf("  section1.key1 = %s\n", ini_get(ini, "section1", "key1"));
    printf("  section1.key2 = %s\n", ini_get(ini, "section1", "key2"));
    printf("  section2.key1 = %s\n", ini_get(ini, "section2", "key1"));

    printf("\n删除 section1.key1:\n");
    ini_delete(ini, "section1", "key1");
    printf("  section1.key1 存在: %s\n",
           ini_has_key(ini, "section1", "key1") ? "是" : "否");

    printf("\n删除整个 section2:\n");
    ini_delete_section(ini, "section2");
    printf("  section2 存在: %s\n",
           ini_has_section(ini, "section2") ? "是" : "否");

    ini_free(ini);
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 应用程序配置\n");
    printf("   - 数据库连接信息\n");
    printf("   - 服务器设置\n");
    printf("   - 日志级别\n");
    printf("\n");

    printf("2. 游戏配置\n");
    printf("   - 图形设置\n");
    printf("   - 音频设置\n");
    printf("   - 控制键位\n");
    printf("\n");

    printf("3. 系统配置\n");
    printf("   - 网络设置\n");
    printf("   - 用户偏好\n");
    printf("   - 插件配置\n");
}

int main(void) {
    printf("========================================\n");
    printf("    INI 配置文件演示\n");
    printf("========================================\n");

    demo_basic();
    demo_types();
    demo_iteration();
    demo_management();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
