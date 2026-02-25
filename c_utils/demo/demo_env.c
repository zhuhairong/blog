/**
 * 环境变量操作演示程序
 * 
 * 功能：
 * - 获取环境变量
 * - 设置和删除环境变量
 * - 类型转换（整数、浮点数、布尔值）
 * - 环境变量展开
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/env.h"

// 演示 1: 获取环境变量
static void demo_get(void) {
    printf("\n=== 演示 1: 获取环境变量 ===\n");
    
    env_options_t opts = env_default_options();
    env_error_t error;
    
    // 获取 PATH
    printf("获取 PATH 环境变量:\n");
    const char *path = env_get("PATH", "", &opts, &error);
    if (error == ENV_OK && path && strlen(path) > 0) {
        printf("  PATH = \"%s...\"\n", path);
        printf("  (长度: %zu 字符)\n", strlen(path));
    } else {
        printf("  未设置或获取失败\n");
    }
    
    // 获取 HOME
    printf("\n获取 HOME 环境变量:\n");
    const char *home = env_get("HOME", "", &opts, &error);
    if (error == ENV_OK && home && strlen(home) > 0) {
        printf("  HOME = \"%s\"\n", home);
    } else {
        printf("  未设置\n");
    }
    
    // 获取不存在的变量（使用默认值）
    printf("\n获取不存在的变量（使用默认值）:\n");
    const char *notexist = env_get("NONEXISTENT_VAR_XYZ", "default_value", &opts, &error);
    printf("  NONEXISTENT_VAR_XYZ = \"%s\"\n", notexist);
    
    // 获取 USER
    printf("\n获取 USER 环境变量:\n");
    const char *user = env_get("USER", "unknown", &opts, &error);
    printf("  USER = \"%s\"\n", user);
}

// 演示 2: 类型转换
static void demo_type_conversion(void) {
    printf("\n=== 演示 2: 类型转换 ===\n");
    
    env_options_t opts = env_default_options();
    env_error_t error;
    
    // 设置测试变量
    setenv("TEST_INT", "42", 1);
    setenv("TEST_FLOAT", "3.14159", 1);
    setenv("TEST_BOOL_TRUE", "true", 1);
    setenv("TEST_BOOL_FALSE", "false", 1);
    setenv("TEST_BOOL_YES", "yes", 1);
    setenv("TEST_BOOL_NO", "0", 1);
    
    // 整数
    printf("整数转换:\n");
    int int_val = env_get_int("TEST_INT", 0, &opts, &error);
    printf("  TEST_INT=\"42\" -> %d\n", int_val);
    
    int int_default = env_get_int("NONEXISTENT_INT", 100, &opts, &error);
    printf("  默认值测试: %d\n", int_default);
    
    // 浮点数
    printf("\n浮点数转换:\n");
    double float_val = env_get_double("TEST_FLOAT", 0.0, &opts, &error);
    printf("  TEST_FLOAT=\"3.14159\" -> %.5f\n", float_val);
    
    // 布尔值
    printf("\n布尔值转换:\n");
    printf("  TEST_BOOL_TRUE=\"true\" -> %s\n", 
           env_get_bool("TEST_BOOL_TRUE", false, &opts, &error) ? "true" : "false");
    printf("  TEST_BOOL_FALSE=\"false\" -> %s\n", 
           env_get_bool("TEST_BOOL_FALSE", true, &opts, &error) ? "true" : "false");
    printf("  TEST_BOOL_YES=\"yes\" -> %s\n", 
           env_get_bool("TEST_BOOL_YES", false, &opts, &error) ? "true" : "false");
    printf("  TEST_BOOL_NO=\"0\" -> %s\n", 
           env_get_bool("TEST_BOOL_NO", true, &opts, &error) ? "true" : "false");
    
    // 清理测试变量
    unsetenv("TEST_INT");
    unsetenv("TEST_FLOAT");
    unsetenv("TEST_BOOL_TRUE");
    unsetenv("TEST_BOOL_FALSE");
    unsetenv("TEST_BOOL_YES");
    unsetenv("TEST_BOOL_NO");
}

// 演示 3: 设置和删除
static void demo_set_unset(void) {
    printf("\n=== 演示 3: 设置和删除环境变量 ===\n");
    
    env_options_t opts = env_default_options();
    env_error_t error;
    
    // 设置变量
    printf("设置环境变量:\n");
    bool result = env_set("MY_APP_VAR", "Hello from demo", true, &opts, &error);
    if (result) {
        printf("  设置 MY_APP_VAR=\"Hello from demo\" - 成功\n");
        
        // 验证
        const char *val = env_get("MY_APP_VAR", "", &opts, &error);
        printf("  验证: MY_APP_VAR=\"%s\"\n", val);
    } else {
        printf("  设置失败: %d\n", error);
    }
    
    // 覆盖变量
    printf("\n覆盖环境变量:\n");
    env_set("MY_APP_VAR", "New value", true, &opts, &error);
    const char *new_val = env_get("MY_APP_VAR", "", &opts, &error);
    printf("  覆盖后: MY_APP_VAR=\"%s\"\n", new_val);
    
    // 不覆盖
    printf("\n不覆盖已存在的变量:\n");
    env_set("MY_APP_VAR", "Should not change", false, &opts, &error);
    const char *unchanged = env_get("MY_APP_VAR", "", &opts, &error);
    printf("  尝试设置但 overwrite=false: MY_APP_VAR=\"%s\"\n", unchanged);
    
    // 删除变量
    printf("\n删除环境变量:\n");
    result = env_unset("MY_APP_VAR", &opts, &error);
    if (result) {
        printf("  删除 MY_APP_VAR - 成功\n");
        
        // 验证
        bool exists = env_has("MY_APP_VAR", &opts, &error);
        printf("  验证存在性: %s\n", exists ? "存在" : "不存在");
    }
}

// 演示 4: 检查存在性
static void demo_check_exists(void) {
    printf("\n=== 演示 4: 检查环境变量存在性 ===\n");
    
    env_options_t opts = env_default_options();
    env_error_t error;
    
    // 检查常见变量
    printf("检查常见环境变量:\n");
    printf("  PATH: %s\n", env_has("PATH", &opts, &error) ? "存在" : "不存在");
    printf("  HOME: %s\n", env_has("HOME", &opts, &error) ? "存在" : "不存在");
    printf("  USER: %s\n", env_has("USER", &opts, &error) ? "存在" : "不存在");
    printf("  SHELL: %s\n", env_has("SHELL", &opts, &error) ? "存在" : "不存在");
    printf("  XYZ_NONEXISTENT: %s\n", env_has("XYZ_NONEXISTENT", &opts, &error) ? "存在" : "不存在");
}

// 演示 5: 环境变量展开
static void demo_expand(void) {
    printf("\n=== 演示 5: 环境变量展开 ===\n");
    
    env_options_t opts = env_default_options();
    env_error_t error;
    
    printf("环境变量展开功能:\n");
    printf("  将字符串中的 $VAR 或 ${VAR} 替换为环境变量值\n");
    printf("\n");
    
    const char *user = env_get("USER", "unknown", &opts, &error);
    const char *home = env_get("HOME", "/home/user", &opts, &error);
    
    printf("示例展开:\n");
    printf("  \"Current user: \$USER\" -> \"Current user: %s\"\n", user);
    printf("  \"Home: \$HOME, User: \$USER\" -> \"Home: %s, User: %s\"\n", home, user);
    printf("  \"\$HOME/Documents\" -> \"%s/Documents\"\n", home);
    
    printf("\n注意: env_expand 函数需要库实现支持\n");
}

// 演示 6: 配置选项
static void demo_options(void) {
    printf("\n=== 演示 6: 配置选项 ===\n");
    
    printf("环境变量选项:\n");
    printf("  expand_variables: 是否展开变量引用\n");
    printf("  trim_whitespace: 是否去除首尾空白\n");
    printf("  case_insensitive: 是否大小写不敏感\n");
    printf("  thread_safe: 是否线程安全\n");
    
    printf("\n默认选项:\n");
    env_options_t opts = env_default_options();
    printf("  expand_variables: %s\n", opts.expand_variables ? "true" : "false");
    printf("  trim_whitespace: %s\n", opts.trim_whitespace ? "true" : "false");
    printf("  case_insensitive: %s\n", opts.case_insensitive ? "true" : "false");
    printf("  thread_safe: %s\n", opts.thread_safe ? "true" : "false");
}

// 演示 7: 实际应用
static void demo_practical_usage(void) {
    printf("\n=== 演示 7: 实际应用场景 ===\n");
    
    printf("1. 应用程序配置\n");
    printf("   - 从环境变量读取配置\n");
    printf("   - 支持类型转换\n");
    printf("   - 提供默认值\n");
    printf("\n");
    
    printf("2. 调试和日志\n");
    printf("   - DEBUG 模式开关\n");
    printf("   - 日志级别设置\n");
    printf("   - 输出路径配置\n");
    printf("\n");
    
    printf("3. 部署配置\n");
    printf("   - 数据库连接字符串\n");
    printf("   - API 密钥和密钥\n");
    printf("   - 服务端点配置\n");
    printf("\n");
    
    printf("4. 路径配置\n");
    printf("   - 配置文件路径\n");
    printf("   - 临时文件目录\n");
    printf("   - 数据存储位置\n");
    
    printf("\n示例配置代码:\n");
    printf("  // 读取应用配置\n");
    printf("  int port = env_get_int(\"APP_PORT\", 8080, NULL, NULL);\n");
    printf("  bool debug = env_get_bool(\"APP_DEBUG\", false, NULL, NULL);\n");
    printf("  const char* db_url = env_get(\"DATABASE_URL\", \"localhost\", NULL, NULL);\n");
}

int main(void) {
    printf("========================================\n");
    printf("    环境变量操作演示程序\n");
    printf("========================================\n");
    
    demo_get();
    demo_type_conversion();
    demo_set_unset();
    demo_check_exists();
    demo_expand();
    demo_options();
    demo_practical_usage();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
