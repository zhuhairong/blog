/**
 * 文件系统工具演示程序
 * 
 * 功能：
 * - 文件读写操作
 * - 文件信息获取
 * - 目录操作
 * - 路径处理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/fs_utils.h"

// 演示 1: 文件读写
static void demo_file_io(void) {
    printf("\n=== 演示 1: 文件读写操作 ===\n");
    
    fs_error_t error;
    fs_options_t opts = fs_default_options();
    
    const char *test_file = "/tmp/fs_utils_test.txt";
    const char *content = "Hello, FS Utils!\nThis is a test file.\nLine 3.";
    
    // 写入文件
    printf("写入文件: %s\n", test_file);
    if (fs_write_all(test_file, content, strlen(content), &opts, &error)) {
        printf("  写入成功 (%zu 字节)\n", strlen(content));
    } else {
        printf("  写入失败: %d\n", error);
        return;
    }
    
    // 读取文件
    printf("\n读取文件:\n");
    size_t size;
    char *data = fs_read_all(test_file, &size, &error);
    if (data) {
        printf("  读取成功 (%zu 字节)\n", size);
        printf("  内容:\n");
        printf("---\n%s\n---\n", data);
        free(data);
    } else {
        printf("  读取失败: %d\n", error);
    }
    
    // 检查文件存在性
    printf("\n检查文件存在性:\n");
    printf("  %s: %s\n", test_file, 
           fs_exists(test_file, &error) ? "存在" : "不存在");
    printf("  /nonexistent/file.txt: %s\n", 
           fs_exists("/nonexistent/file.txt", &error) ? "存在" : "不存在");
    
    // 获取文件大小
    printf("\n获取文件大小:\n");
    long file_size = fs_file_size(test_file, &error);
    if (file_size >= 0) {
        printf("  %s: %ld 字节\n", test_file, file_size);
    }
    
    // 获取文件扩展名
    printf("\n获取文件扩展名:\n");
    const char *ext = fs_extname(test_file, &error);
    if (ext) {
        printf("  %s 的扩展名: \"%s\"\n", test_file, ext);
    }
    
    printf("  /path/to/file.tar.gz 的扩展名: \"%s\"\n", 
           fs_extname("/path/to/file.tar.gz", &error));
    printf("  /path/to/README 的扩展名: \"%s\"\n", 
           fs_extname("/path/to/README", &error));
}

// 演示 2: 文件信息
static void demo_file_info(void) {
    printf("\n=== 演示 2: 文件信息获取 ===\n");
    
    fs_error_t error;
    
    printf("文件类型枚举:\n");
    printf("  FS_TYPE_UNKNOWN      - 未知\n");
    printf("  FS_TYPE_REGULAR      - 普通文件\n");
    printf("  FS_TYPE_DIRECTORY    - 目录\n");
    printf("  FS_TYPE_SYMLINK      - 符号链接\n");
    printf("  FS_TYPE_CHAR_DEVICE  - 字符设备\n");
    printf("  FS_TYPE_BLOCK_DEVICE - 块设备\n");
    printf("  FS_TYPE_FIFO         - 命名管道\n");
    printf("  FS_TYPE_SOCKET       - 套接字\n");
    
    printf("\n获取文件信息示例:\n");
    printf("  文件: /etc/passwd\n");
    
    // 这里只是示例，实际 API 可能需要调整
    printf("  类型: 普通文件\n");
    printf("  大小: 查看实际文件\n");
    printf("  权限: 查看实际文件\n");
}

// 演示 3: 路径操作
static void demo_path_operations(void) {
    printf("\n=== 演示 3: 路径操作 ===\n");
    
    printf("路径处理功能:\n");
    printf("  - 路径规范化\n");
    printf("  - 路径拼接\n");
    printf("  - 相对路径转绝对路径\n");
    printf("  - 获取父目录\n");
    printf("  - 获取文件名\n");
    
    printf("\n路径示例:\n");
    printf("  /home/user/documents/file.txt\n");
    printf("    父目录: /home/user/documents\n");
    printf("    文件名: file.txt\n");
    printf("    扩展名: .txt\n");
    printf("    基本名: file\n");
    
    printf("\n  ./relative/path/../file.txt\n");
    printf("    规范化: ./relative/file.txt\n");
    
    printf("\n  ~/Documents\n");
    printf("    展开: /home/username/Documents\n");
}

// 演示 4: 目录操作
static void demo_directory_ops(void) {
    printf("\n=== 演示 4: 目录操作 ===\n");
    
    fs_error_t error;
    
    printf("目录操作功能:\n");
    printf("  - 创建目录\n");
    printf("  - 删除目录\n");
    printf("  - 遍历目录\n");
    printf("  - 递归操作\n");
    
    printf("\n创建目录示例:\n");
    printf("  fs_mkdir(\"/tmp/test_dir\", 0755, &error)\n");
    printf("  fs_mkdir_recursive(\"/tmp/a/b/c\", 0755, &error)\n");
    
    printf("\n遍历目录示例:\n");
    printf("  fs_dir_foreach(\"/tmp\", callback, &error)\n");
    printf("    遍历 /tmp 目录中的所有文件\n");
}

// 演示 5: 文件系统选项
static void demo_options(void) {
    printf("\n=== 演示 5: 文件系统选项 ===\n");
    
    fs_options_t opts = fs_default_options();
    
    printf("默认选项:\n");
    printf("  follow_symlinks: %s\n", 
           opts.follow_symlinks ? "true" : "false");
    printf("  create_dirs: %s\n", 
           opts.create_dirs ? "true" : "false");
    printf("  atomic_write: %s\n", 
           opts.atomic_write ? "true" : "false");
    printf("  secure_permissions: %s\n", 
           opts.secure_permissions ? "true" : "false");
    printf("  buffer_size: %zu\n", opts.buffer_size);
    
    printf("\n选项说明:\n");
    printf("  follow_symlinks:\n");
    printf("    - true: 跟随符号链接\n");
    printf("    - false: 操作链接本身\n");
    printf("\n");
    printf("  create_dirs:\n");
    printf("    - true: 自动创建父目录\n");
    printf("    - false: 父目录必须存在\n");
    printf("\n");
    printf("  atomic_write:\n");
    printf("    - true: 原子写入 (先写临时文件再重命名)\n");
    printf("    - false: 直接写入\n");
    printf("\n");
    printf("  secure_permissions:\n");
    printf("    - true: 使用安全权限 (0600/0700)\n");
    printf("    - false: 使用默认权限\n");
}

// 演示 6: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");
    
    printf("错误码:\n");
    printf("  FS_OK (0): 成功\n");
    printf("  FS_ERROR_INVALID_PARAM: 无效参数\n");
    printf("  FS_ERROR_FILE_NOT_FOUND: 文件不存在\n");
    printf("  FS_ERROR_FILE_OPEN: 打开文件失败\n");
    printf("  FS_ERROR_FILE_READ: 读取文件失败\n");
    printf("  FS_ERROR_FILE_WRITE: 写入文件失败\n");
    printf("  FS_ERROR_PERMISSION_DENIED: 权限拒绝\n");
    printf("  FS_ERROR_MEMORY_ALLOC: 内存分配失败\n");
    
    printf("\n错误处理示例:\n");
    printf("  fs_error_t error;\n");
    printf("  char *data = fs_read_all(\"/etc/shadow\", &size, &error);\n");
    printf("  if (!data) {\n");
    printf("    if (error == FS_ERROR_PERMISSION_DENIED) {\n");
    printf("      printf(\"权限不足\\n\");\n");
    printf("    } else if (error == FS_ERROR_FILE_NOT_FOUND) {\n");
    printf("      printf(\"文件不存在\\n\");\n");
    printf("    }\n");
    printf("  }\n");
}

// 演示 7: 实际应用
static void demo_applications(void) {
    printf("\n=== 演示 7: 实际应用场景 ===\n");
    
    printf("1. 配置文件管理\n");
    printf("   - 读取配置文件\n");
    printf("   - 自动创建默认配置\n");
    printf("   - 原子写入更新\n");
    printf("\n");
    
    printf("2. 日志系统\n");
    printf("   - 日志文件写入\n");
    printf("   - 日志轮转\n");
    printf("   - 目录管理\n");
    printf("\n");
    
    printf("3. 缓存系统\n");
    printf("   - 缓存文件读写\n");
    printf("   - 缓存清理\n");
    printf("   - 过期检测\n");
    printf("\n");
    
    printf("4. 文件同步\n");
    printf("   - 文件复制\n");
    printf("   - 目录同步\n");
    printf("   - 增量更新\n");
    printf("\n");
    
    printf("5. 安装程序\n");
    printf("   - 创建目录结构\n");
    printf("   - 复制文件\n");
    printf("   - 设置权限\n");
}

int main(void) {
    printf("========================================\n");
    printf("    文件系统工具演示程序\n");
    printf("========================================\n");
    
    demo_file_io();
    demo_file_info();
    demo_path_operations();
    demo_directory_ops();
    demo_options();
    demo_error_handling();
    demo_applications();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
