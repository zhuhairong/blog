/**
 * 路径处理演示程序
 *
 * 功能：
 * - 路径拼接
 * - 路径解析
 * - 路径规范化
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/path.h"

// 演示 1: 路径拼接
static void demo_join(void) {
    printf("\n=== 演示 1: 路径拼接 ===\n");

    printf("系统路径分隔符: '%c'\n\n", path_get_separator());

    struct {
        const char *p1;
        const char *p2;
    } cases[] = {
        {"/home/user", "documents"},
        {"home/user", "file.txt"},
        {"/usr/local/", "bin"},
        {NULL, NULL}
    };

    printf("路径拼接示例:\n\n");

    for (int i = 0; cases[i].p1; i++) {
        char *result = path_join(cases[i].p1, cases[i].p2);
        if (result) {
            printf("  \"%s\" + \"%s\" = \"%s\"\n",
                   cases[i].p1, cases[i].p2, result);
            free(result);
        }
    }
}

// 演示 2: 获取目录名
static void demo_dirname(void) {
    printf("\n=== 演示 2: 获取目录名 ===\n");

    const char *paths[] = {
        "/home/user/documents/file.txt",
        "/usr/local/bin",
        "file.txt",
        "/",
        NULL
    };

    printf("提取目录名:\n\n");

    for (int i = 0; paths[i]; i++) {
        char *dir = path_dirname(paths[i]);
        if (dir) {
            printf("  \"%s\" -> \"%s\"\n", paths[i], dir);
            free(dir);
        }
    }
}

// 演示 3: 获取文件名
static void demo_basename(void) {
    printf("\n=== 演示 3: 获取文件名 ===\n");

    const char *paths[] = {
        "/home/user/documents/file.txt",
        "/usr/local/bin/myapp",
        "file.txt",
        "/",
        NULL
    };

    printf("提取文件名:\n\n");

    for (int i = 0; paths[i]; i++) {
        const char *base = path_basename(paths[i]);
        printf("  \"%s\" -> \"%s\"\n", paths[i], base);
    }
}

// 演示 4: 获取扩展名
static void demo_extension(void) {
    printf("\n=== 演示 4: 获取扩展名 ===\n");

    const char *paths[] = {
        "document.txt",
        "/home/user/image.png",
        "archive.tar.gz",
        "Makefile",
        NULL
    };

    printf("提取扩展名:\n\n");

    for (int i = 0; paths[i]; i++) {
        const char *ext = path_get_extension(paths[i]);
        printf("  \"%s\" -> \"%s\"\n", paths[i], ext);
    }
}

// 演示 5: 路径规范化
static void demo_normalize(void) {
    printf("\n=== 演示 5: 路径规范化 ===\n");

    const char *paths[] = {
        "/home/user/../documents",
        "/home/./user//documents",
        "home/user/../../etc",
        NULL
    };

    printf("路径规范化:\n\n");

    for (int i = 0; paths[i]; i++) {
        path_error_t error;
        char *norm = path_normalize(paths[i], &error);
        if (norm) {
            printf("  \"%s\" -> \"%s\"\n", paths[i], norm);
            free(norm);
        }
    }
}

// 演示 6: 路径信息
static void demo_info(void) {
    printf("\n=== 演示 6: 路径信息 ===\n");

    const char *path = "/home/user/documents/file.txt";

    printf("路径: \"%s\"\n\n", path);

    path_info_t info;
    path_error_t error;

    if (path_parse(path, &info, &error)) {
        printf("路径信息:\n");
        printf("  目录: %s\n", info.directory ? info.directory : "(空)");
        printf("  文件名: %s\n", info.filename ? info.filename : "(空)");
        printf("  扩展名: %s\n", info.extension ? info.extension : "(空)");
        printf("  绝对路径: %s\n", info.is_absolute ? "是" : "否");
        printf("  目录路径: %s\n", info.is_directory ? "是" : "否");
        printf("  存在: %s\n", info.exists ? "是" : "否");

        path_free_info(&info);
    }
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 配置文件路径\n");
    printf("   - 拼接配置目录和文件名\n");
    printf("   - 跨平台路径处理\n\n");

    printf("2. 日志文件管理\n");
    printf("   - 生成日志文件路径\n");
    printf("   - 按日期组织目录\n\n");

    printf("3. 资源加载\n");
    printf("   - 查找资源文件\n");
    printf("   - 处理相对路径\n\n");

    printf("4. 文件浏览器\n");
    printf("   - 显示文件名\n");
    printf("   - 提取扩展名过滤\n\n");

    printf("5. 构建工具\n");
    printf("   - 处理源文件路径\n");
    printf("   - 生成输出路径\n");
}

int main(void) {
    printf("========================================\n");
    printf("    路径处理演示\n");
    printf("========================================\n");

    demo_join();
    demo_dirname();
    demo_basename();
    demo_extension();
    demo_normalize();
    demo_info();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
