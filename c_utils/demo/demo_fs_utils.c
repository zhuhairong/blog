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
#include <time.h>
#include <sys/stat.h>
#include "../c_utils/fs_utils.h"

static void demo_file_io(void) {
    printf("\n=== 演示 1: 文件读写操作 ===\n");
    
    fs_error_t error;
    fs_options_t opts = fs_default_options();
    
    const char *test_file = "/tmp/fs_utils_test.txt";
    const char *content = "Hello, FS Utils!\nThis is a test file.\nLine 3.";
    
    printf("写入文件: %s\n", test_file);
    if (fs_write_all(test_file, content, strlen(content), &opts, &error)) {
        printf("  写入成功 (%zu 字节)\n", strlen(content));
    } else {
        printf("  写入失败: %d\n", error);
        return;
    }
    
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
    
    printf("\n检查文件存在性:\n");
    printf("  %s: %s\n", test_file, 
           fs_exists(test_file, &error) ? "存在" : "不存在");
    printf("  /nonexistent/file.txt: %s\n", 
           fs_exists("/nonexistent/file.txt", &error) ? "存在" : "不存在");
    
    printf("\n获取文件大小:\n");
    long file_size = fs_file_size(test_file, &error);
    if (file_size >= 0) {
        printf("  %s: %ld 字节\n", test_file, file_size);
    }
    
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

static const char* file_type_to_string(fs_file_type_t type) {
    switch (type) {
        case FS_TYPE_REGULAR:      return "普通文件";
        case FS_TYPE_DIRECTORY:    return "目录";
        case FS_TYPE_SYMLINK:      return "符号链接";
        case FS_TYPE_CHAR_DEVICE:  return "字符设备";
        case FS_TYPE_BLOCK_DEVICE: return "块设备";
        case FS_TYPE_FIFO:         return "命名管道";
        case FS_TYPE_SOCKET:       return "套接字";
        default:                   return "未知";
    }
}

static void demo_fs_stat(void) {
    printf("\n=== 演示 2: fs_stat - 获取文件详细信息 ===\n");
    
    fs_error_t error;
    fs_file_info_t info;
    
    const char *test_files[] = {
        "/tmp/fs_utils_test.txt",
        "/tmp",
        "/dev/null",
        "/etc/passwd"
    };
    
    for (size_t i = 0; i < sizeof(test_files) / sizeof(test_files[0]); i++) {
        const char *filepath = test_files[i];
        printf("\n文件: %s\n", filepath);
        
        if (fs_stat(filepath, &info, &error)) {
            printf("  类型: %s\n", file_type_to_string(info.type));
            printf("  大小: %zu 字节\n", info.size);
            printf("  权限: %04o (八进制)\n", info.mode & 07777);
            
            char time_buf[64];
            struct tm *tm_info;
            
            tm_info = localtime(&info.mtime);
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("  修改时间: %s\n", time_buf);
            
            tm_info = localtime(&info.atime);
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("  访问时间: %s\n", time_buf);
            
            tm_info = localtime(&info.ctime);
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("  状态时间: %s\n", time_buf);
            
            printf("  UID: %u, GID: %u\n", info.uid, info.gid);
            
            if (info.path) {
                printf("  路径: %s\n", info.path);
            }
        } else {
            printf("  获取信息失败: %s\n", fs_strerror(error));
        }
    }
}

static void demo_fs_read_dir(void) {
    printf("\n=== 演示 3: fs_read_dir - 列出目录内容 ===\n");
    
    fs_error_t error;
    char **entries = NULL;
    size_t count = 0;
    
    const char *dirs[] = {"/tmp", "/dev"};
    
    for (size_t d = 0; d < sizeof(dirs) / sizeof(dirs[0]); d++) {
        const char *dirpath = dirs[d];
        printf("\n目录: %s\n", dirpath);
        
        if (fs_read_dir(dirpath, &entries, &count, &error)) {
            printf("  共 %zu 个条目:\n", count);
            
            size_t display_count = count > 10 ? 10 : count;
            for (size_t i = 0; i < display_count; i++) {
                printf("    [%zu] %s\n", i + 1, entries[i]);
            }
            if (count > 10) {
                printf("    ... 还有 %zu 个条目\n", count - 10);
            }
            
            fs_free_dir_entries(&entries, count);
        } else {
            printf("  读取目录失败: %s\n", fs_strerror(error));
        }
    }
}

static void demo_fs_strerror(void) {
    printf("\n=== 演示 4: fs_strerror - 错误信息获取 ===\n");
    
    fs_error_t error;
    
    printf("错误码与对应信息:\n");
    
    fs_error_t errors[] = {
        FS_OK,
        FS_ERROR_INVALID_PARAM,
        FS_ERROR_FILE_NOT_FOUND,
        FS_ERROR_FILE_OPEN,
        FS_ERROR_FILE_READ,
        FS_ERROR_FILE_WRITE,
        FS_ERROR_FILE_CREATE,
        FS_ERROR_FILE_DELETE,
        FS_ERROR_DIR_OPEN,
        FS_ERROR_DIR_CREATE,
        FS_ERROR_DIR_DELETE,
        FS_ERROR_DIR_READ,
        FS_ERROR_PERMISSION_DENIED,
        FS_ERROR_MEMORY_ALLOC,
        FS_ERROR_BUFFER_TOO_SMALL,
        FS_ERROR_PLATFORM_UNSUPPORTED,
        FS_ERROR_INVALID_PATH
    };
    
    for (size_t i = 0; i < sizeof(errors) / sizeof(errors[0]); i++) {
        printf("  [%2d] %s\n", errors[i], fs_strerror(errors[i]));
    }
    
    printf("\n实际使用示例:\n");
    printf("  尝试读取不存在的文件...\n");
    size_t size;
    char *data = fs_read_all("/nonexistent/path/to/file.txt", &size, &error);
    if (!data) {
        printf("  错误码: %d\n", error);
        printf("  错误信息: %s\n", fs_strerror(error));
    }
}

static void demo_fs_is_absolute_path(void) {
    printf("\n=== 演示 5: fs_is_absolute_path - 绝对路径判断 ===\n");
    
    const char *paths[] = {
        "/home/user/documents",
        "/tmp/test.txt",
        "./relative/path",
        "../parent/dir",
        "simple_filename.txt",
        "~/home_directory",
        "/",
        "C:\\Windows\\System32",
        "D:/data/files",
        ""
    };
    
    printf("路径判断结果:\n");
    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        bool is_abs = fs_is_absolute_path(paths[i]);
        printf("  \"%s\" -> %s\n", 
               paths[i], 
               is_abs ? "绝对路径" : "相对路径");
    }
}

static void demo_fs_realpath(void) {
    printf("\n=== 演示 6: fs_realpath - 路径规范化 ===\n");
    
    fs_error_t error;
    char buffer[1024];
    
    const char *paths[] = {
        "/tmp/../tmp/./test.txt",
        "/home/user/../user/./documents",
        "./current_dir",
        "/tmp/does_not_exist",
        "/dev/null"
    };
    
    printf("路径规范化结果:\n");
    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        const char *path = paths[i];
        char *resolved = fs_realpath(path, buffer, sizeof(buffer), &error);
        
        if (resolved) {
            printf("  \"%s\"\n", path);
            printf("    -> \"%s\"\n", resolved);
        } else {
            printf("  \"%s\"\n", path);
            printf("    -> 解析失败: %s\n", fs_strerror(error));
        }
    }
}

static void demo_fs_getcwd_chdir(void) {
    printf("\n=== 演示 7: fs_getcwd/fs_chdir - 工作目录操作 ===\n");
    
    fs_error_t error;
    char cwd_buffer[1024];
    
    char *original_cwd = fs_getcwd(cwd_buffer, sizeof(cwd_buffer), &error);
    if (original_cwd) {
        printf("当前工作目录: %s\n", original_cwd);
    } else {
        printf("获取当前目录失败: %s\n", fs_strerror(error));
        return;
    }
    
    printf("\n切换到 /tmp 目录...\n");
    if (fs_chdir("/tmp", &error)) {
        char new_cwd[1024];
        if (fs_getcwd(new_cwd, sizeof(new_cwd), &error)) {
            printf("  新工作目录: %s\n", new_cwd);
        }
        
        printf("\n切换回原目录...\n");
        if (fs_chdir(original_cwd, &error)) {
            char restored_cwd[1024];
            if (fs_getcwd(restored_cwd, sizeof(restored_cwd), &error)) {
                printf("  恢复后的工作目录: %s\n", restored_cwd);
            }
        } else {
            printf("  切换失败: %s\n", fs_strerror(error));
        }
    } else {
        printf("  切换失败: %s\n", fs_strerror(error));
    }
    
    printf("\n尝试切换到不存在的目录...\n");
    if (!fs_chdir("/nonexistent/directory", &error)) {
        printf("  预期失败: %s\n", fs_strerror(error));
    }
}

static void demo_fs_rmdir_recursive(void) {
    printf("\n=== 演示 8: fs_rmdir recursive - 递归删除目录 ===\n");
    
    fs_error_t error;
    
    const char *test_dir = "/tmp/fs_utils_test_dir";
    const char *sub_dirs[] = {
        "/tmp/fs_utils_test_dir",
        "/tmp/fs_utils_test_dir/subdir1",
        "/tmp/fs_utils_test_dir/subdir2",
        "/tmp/fs_utils_test_dir/subdir1/deep"
    };
    
    printf("创建测试目录结构...\n");
    for (int i = 0; i < 4; i++) {
        if (fs_mkdir(sub_dirs[i], true, &error)) {
            printf("  创建: %s\n", sub_dirs[i]);
        }
    }
    
    const char *test_files[] = {
        "/tmp/fs_utils_test_dir/file1.txt",
        "/tmp/fs_utils_test_dir/subdir1/file2.txt",
        "/tmp/fs_utils_test_dir/subdir2/file3.txt",
        "/tmp/fs_utils_test_dir/subdir1/deep/file4.txt"
    };
    
    fs_options_t opts = fs_default_options();
    for (int i = 0; i < 4; i++) {
        const char *content = "test content";
        if (fs_write_all(test_files[i], content, strlen(content), &opts, &error)) {
            printf("  创建文件: %s\n", test_files[i]);
        }
    }
    
    printf("\n目录结构创建完成\n");
    
    printf("\n尝试非递归删除非空目录...\n");
    if (!fs_rmdir(test_dir, false, &error)) {
        printf("  预期失败: %s\n", fs_strerror(error));
    }
    
    printf("\n递归删除目录...\n");
    if (fs_rmdir(test_dir, true, &error)) {
        printf("  删除成功: %s\n", test_dir);
        printf("  验证目录是否已删除: %s\n", 
               fs_exists(test_dir, &error) ? "仍存在" : "已删除");
    } else {
        printf("  删除失败: %s\n", fs_strerror(error));
    }
}

static void demo_fs_copy_binary(void) {
    printf("\n=== 演示 9: fs_copy - 二进制文件复制 ===\n");
    
    fs_error_t error;
    fs_options_t opts = fs_default_options();
    
    const char *src_file = "/tmp/fs_utils_binary_test.bin";
    const char *dest_file = "/tmp/fs_utils_binary_copy.bin";
    
    printf("创建测试二进制文件...\n");
    unsigned char binary_data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8,
        0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70
    };
    
    if (fs_write_all(src_file, binary_data, sizeof(binary_data), &opts, &error)) {
        printf("  创建源文件: %s (%zu 字节)\n", src_file, sizeof(binary_data));
    } else {
        printf("  创建源文件失败: %s\n", fs_strerror(error));
        return;
    }
    
    printf("\n复制二进制文件...\n");
    if (fs_copy(src_file, dest_file, &opts, &error)) {
        printf("  复制成功: %s -> %s\n", src_file, dest_file);
        
        printf("\n验证复制结果:\n");
        size_t src_size, dest_size;
        char *src_data = fs_read_all(src_file, &src_size, &error);
        char *dest_data = fs_read_all(dest_file, &dest_size, &error);
        
        if (src_data && dest_data) {
            printf("  源文件大小: %zu 字节\n", src_size);
            printf("  目标文件大小: %zu 字节\n", dest_size);
            
            if (src_size == dest_size && memcmp(src_data, dest_data, src_size) == 0) {
                printf("  内容验证: 完全一致 ✓\n");
            } else {
                printf("  内容验证: 不一致 ✗\n");
            }
            
            free(src_data);
            free(dest_data);
        }
        
        printf("\n显示二进制内容 (前16字节):\n");
        printf("  源文件:   ");
        for (size_t i = 0; i < 16 && i < src_size; i++) {
            printf("%02X ", (unsigned char)src_data[i]);
        }
        printf("\n");
    } else {
        printf("  复制失败: %s\n", fs_strerror(error));
    }
    
    printf("\n清理测试文件...\n");
    fs_unlink(src_file, &error);
    fs_unlink(dest_file, &error);
    printf("  清理完成\n");
}

static void demo_options(void) {
    printf("\n=== 演示 10: 文件系统选项 ===\n");
    
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

int main(void) {
    printf("========================================\n");
    printf("    文件系统工具演示程序\n");
    printf("========================================\n");
    
    demo_file_io();
    demo_fs_stat();
    demo_fs_read_dir();
    demo_fs_strerror();
    demo_fs_is_absolute_path();
    demo_fs_realpath();
    demo_fs_getcwd_chdir();
    demo_fs_rmdir_recursive();
    demo_fs_copy_binary();
    demo_options();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
