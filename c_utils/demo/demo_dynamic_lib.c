/**
 * 动态库加载演示程序
 * 
 * 功能：
 * - 打开和关闭动态库
 * - 获取符号（函数和变量）
 * - 错误处理
 * - 跨平台注意事项
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/dynamic_lib.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");
    
    printf("动态库加载流程:\n");
    printf("  1. 打开动态库\n");
    printf("     dynamic_lib_open(\"libxxx.so\", &error)\n");
    printf("\n");
    printf("  2. 获取符号\n");
    printf("     dynamic_lib_get_symbol(lib, \"func_name\", &error)\n");
    printf("\n");
    printf("  3. 使用符号\n");
    printf("     调用函数或访问变量\n");
    printf("\n");
    printf("  4. 关闭动态库\n");
    printf("     dynamic_lib_close(lib, &error)\n");
    
    printf("\n不同平台的库文件扩展名:\n");
    printf("  Linux:   .so  (Shared Object)\n");
    printf("  macOS:   .dylib (Dynamic Library)\n");
    printf("  Windows: .dll (Dynamic Link Library)\n");
}

// 演示 2: 加载系统库
static void demo_load_system_lib(void) {
    printf("\n=== 演示 2: 加载系统库 ===\n");
    
    dynamic_lib_error_t error;
    
    // 尝试加载标准 C 库
    printf("尝试加载 C 标准库:\n");
    
    const char *libc_paths[] = {
        "libc.so.6",      // Linux glibc
        "libc.so",        // Linux generic
        "/lib/libc.so.6", // Linux full path
    };
    
    dynamic_lib_t *lib = NULL;
    for (size_t i = 0; i < sizeof(libc_paths)/sizeof(libc_paths[0]); i++) {
        printf("  尝试: %s ... ", libc_paths[i]);
        lib = dynamic_lib_open(libc_paths[i], &error);
        if (lib) {
            printf("成功!\n");
            break;
        } else {
            printf("失败\n");
        }
    }
    
    if (lib) {
        printf("\n库信息:\n");
        printf("  路径: %s\n", dynamic_lib_get_path(lib));
        printf("  是否有效: %s\n", dynamic_lib_is_valid(lib) ? "是" : "否");
        
        // 尝试获取 printf 符号
        printf("\n获取 printf 符号:\n");
        void *printf_sym = dynamic_lib_get_symbol(lib, "printf", &error);
        if (printf_sym) {
            printf("  成功获取 printf 地址: %p\n", printf_sym);
        } else {
            printf("  获取失败 (错误码: %d)\n", error);
        }
        
        // 尝试获取 malloc 符号
        printf("\n获取 malloc 符号:\n");
        void *malloc_sym = dynamic_lib_get_symbol(lib, "malloc", &error);
        if (malloc_sym) {
            printf("  成功获取 malloc 地址: %p\n", malloc_sym);
        }
        
        dynamic_lib_close(lib, &error);
        printf("\n库已关闭\n");
    } else {
        printf("\n无法加载 C 标准库 (这在某些环境中是正常的)\n");
    }
}

// 演示 3: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 3: 错误处理 ===\n");
    
    dynamic_lib_error_t error;
    
    // 尝试打开不存在的库
    printf("尝试打开不存在的库:\n");
    dynamic_lib_t *lib = dynamic_lib_open("/nonexistent/library.so", &error);
    
    if (!lib) {
        printf("  预期中的失败\n");
        printf("  错误码: %d\n", error);
    }
    
    // 打开一个存在的库然后获取不存在的符号
    printf("\n尝试获取不存在的符号:\n");
    lib = dynamic_lib_open("libc.so.6", &error);
    if (lib) {
        void *sym = dynamic_lib_get_symbol(lib, "nonexistent_function_xyz", &error);
        if (!sym) {
            printf("  预期中的失败\n");
            printf("  错误码: %d\n", error);
        }
        dynamic_lib_close(lib, &error);
    }
    
    printf("\n错误码说明:\n");
    printf("  DYNAMIC_LIB_OK (0): 成功\n");
    printf("  DYNAMIC_LIB_ERROR_INVALID_PARAM (-1): 无效参数\n");
    printf("  DYNAMIC_LIB_ERROR_OPEN_FAILED (-2): 打开失败\n");
    printf("  DYNAMIC_LIB_ERROR_SYMBOL_NOT_FOUND (-3): 符号未找到\n");
    printf("  DYNAMIC_LIB_ERROR_CLOSE_FAILED (-4): 关闭失败\n");
}

// 演示 4: 创建测试库（概念演示）
static void demo_create_plugin(void) {
    printf("\n=== 演示 4: 插件系统概念 ===\n");
    
    printf("插件系统架构:\n");
    printf("  主程序 (Host Application)\n");
    printf("    |\n");
    printf("    +-- 加载插件管理器\n");
    printf("    |\n");
    printf("    +-- 扫描插件目录\n");
    printf("    |\n");
    printf("    +-- 加载插件 .so/.dll 文件\n");
    printf("    |   +-- dynamic_lib_open()\n");
    printf("    |   +-- 获取插件接口\n");
    printf("    |   +-- 初始化插件\n");
    printf("    |\n");
    printf("    +-- 使用插件功能\n");
    printf("    |\n");
    printf("    +-- 卸载插件\n");
    printf("        +-- 清理插件\n");
    printf("        +-- dynamic_lib_close()\n");
    
    printf("\n插件接口示例:\n");
    printf("  // plugin.h\n");
    printf("  typedef struct {\n");
    printf("      int version;\n");
    printf("      const char* name;\n");
    printf("      void (*init)(void);\n");
    printf("      void (*cleanup)(void);\n");
    printf("      void (*execute)(const char* args);\n");
    printf("  } plugin_interface_t;\n");
    printf("\n");
    printf("  // plugin.c\n");
    printf("  plugin_interface_t plugin_info = {\n");
    printf("      .version = 1,\n");
    printf("      .name = \"Sample Plugin\",\n");
    printf("      .init = sample_init,\n");
    printf("      .cleanup = sample_cleanup,\n");
    printf("      .execute = sample_execute\n");
    printf("  };\n");
}

// 演示 5: 使用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 5: 使用场景 ===\n");
    
    printf("1. 插件系统\n");
    printf("   - 应用程序扩展机制\n");
    printf("   - 动态加载功能模块\n");
    printf("   - 第三方扩展支持\n");
    printf("\n");
    
    printf("2. 驱动加载\n");
    printf("   - 硬件驱动动态加载\n");
    printf("   - 设备热插拔支持\n");
    printf("\n");
    
    printf("3. 脚本引擎\n");
    printf("   - 动态加载脚本解释器\n");
    printf("   - 支持多种脚本语言\n");
    printf("\n");
    
    printf("4. 数据库驱动\n");
    printf("   - 按需加载数据库连接器\n");
    printf("   - 支持多种数据库\n");
    printf("\n");
    
    printf("5. 编解码器\n");
    printf("   - 动态加载音频/视频编解码器\n");
    printf("   - 支持新格式无需重新编译\n");
}

// 演示 6: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 6: 最佳实践 ===\n");
    
    printf("1. 错误处理\n");
    printf("   - 始终检查返回值\n");
    printf("   - 使用错误码获取详细信息\n");
    printf("   - 优雅处理库加载失败\n");
    printf("\n");
    
    printf("2. 资源管理\n");
    printf("   - 确保关闭所有打开的库\n");
    printf("   - 避免内存泄漏\n");
    printf("   - 使用 RAII 模式\n");
    printf("\n");
    
    printf("3. 符号解析\n");
    printf("   - 检查符号是否存在\n");
    printf("   - 验证符号类型\n");
    printf("   - 使用显式类型转换\n");
    printf("\n");
    
    printf("4. 跨平台\n");
    printf("   - 使用条件编译处理平台差异\n");
    printf("   - 抽象平台相关代码\n");
    printf("   - 测试所有目标平台\n");
    printf("\n");
    
    printf("5. 安全性\n");
    printf("   - 验证库文件来源\n");
    printf("   - 检查库文件签名\n");
    printf("   - 限制库文件权限\n");
}

int main(void) {
    printf("========================================\n");
    printf("    动态库加载演示程序\n");
    printf("========================================\n");
    
    demo_basic();
    demo_load_system_lib();
    demo_error_handling();
    demo_create_plugin();
    demo_use_cases();
    demo_best_practices();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
