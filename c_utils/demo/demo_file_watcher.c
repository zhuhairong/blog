/**
 * 文件监控演示程序
 * 
 * 功能：
 * - 监控文件变化
 * - 添加和移除监控项
 * - 事件回调处理
 * - 批量监控
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/file_watcher.h"

// 事件回调函数
static void event_callback(const char *path, file_watcher_event_t event, void *user_data) {
    const char *event_name;
    switch (event) {
        case FILE_WATCHER_EVENT_CREATE: event_name = "创建"; break;
        case FILE_WATCHER_EVENT_MODIFY: event_name = "修改"; break;
        case FILE_WATCHER_EVENT_DELETE: event_name = "删除"; break;
        case FILE_WATCHER_EVENT_RENAME: event_name = "重命名"; break;
        case FILE_WATCHER_EVENT_ERROR: event_name = "错误"; break;
        default: event_name = "未知"; break;
    }
    printf("  [回调] 文件: %s, 事件: %s\n", path, event_name);
    
    if (user_data) {
        printf("  [回调] 用户数据: %s\n", (char*)user_data);
    }
}

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");
    
    file_watcher_error_t error;
    
    printf("创建文件监控器:\n");
    file_watcher_options_t opts = file_watcher_default_options();
    printf("  默认选项:\n");
    printf("    recursive: %s\n", opts.recursive ? "true" : "false");
    printf("    follow_symlinks: %s\n", opts.follow_symlinks ? "true" : "false");
    printf("    watch_hidden: %s\n", opts.watch_hidden ? "true" : "false");
    printf("    use_polling: %s\n", opts.use_polling ? "true" : "false");
    printf("    poll_interval_ms: %d\n", opts.poll_interval_ms);
    
    file_watcher_t *fw = file_watcher_create(&opts, event_callback, "user_data_demo", &error);
    if (!fw) {
        printf("  创建失败: %d\n", error);
        return;
    }
    
    printf("\n  监控器创建成功\n");
    
    // 添加监控项
    printf("\n添加监控项:\n");
    printf("  添加 /tmp/test_file.txt\n");
    if (file_watcher_add(fw, "/tmp/test_file.txt", &error)) {
        printf("  添加成功\n");
    } else {
        printf("  添加失败: %d\n", error);
    }
    
    printf("  添加 /tmp/test_dir\n");
    if (file_watcher_add(fw, "/tmp/test_dir", &error)) {
        printf("  添加成功\n");
    } else {
        printf("  添加失败: %d (目录可能不存在)\n", error);
    }
    
    // 检查文件变化
    printf("\n检查文件变化:\n");
    if (file_watcher_check(fw, &error)) {
        printf("  检查完成\n");
    } else {
        printf("  检查失败: %d\n", error);
    }
    
    // 移除监控项
    printf("\n移除监控项:\n");
    if (file_watcher_remove(fw, "/tmp/test_file.txt", &error)) {
        printf("  移除成功\n");
    } else {
        printf("  移除失败: %d\n", error);
    }
    
    file_watcher_free(fw);
    printf("\n  监控器已释放\n");
}

// 演示 2: 事件类型
static void demo_event_types(void) {
    printf("\n=== 演示 2: 事件类型 ===\n");
    
    printf("文件监控事件类型:\n");
    printf("  FILE_WATCHER_EVENT_CREATE - 文件创建\n");
    printf("  FILE_WATCHER_EVENT_MODIFY - 文件修改\n");
    printf("  FILE_WATCHER_EVENT_DELETE - 文件删除\n");
    printf("  FILE_WATCHER_EVENT_RENAME - 文件重命名\n");
    printf("  FILE_WATCHER_EVENT_ERROR  - 监控错误\n");
    
    printf("\n使用场景:\n");
    printf("  CREATE - 新文件到达通知\n");
    printf("  MODIFY - 配置文件热重载\n");
    printf("  DELETE - 资源清理\n");
    printf("  RENAME - 日志轮转检测\n");
}

// 演示 3: 批量监控
static void demo_batch_watch(void) {
    printf("\n=== 演示 3: 批量监控 ===\n");
    
    file_watcher_error_t error;
    file_watcher_t *fw = file_watcher_create(NULL, NULL, NULL, &error);
    if (!fw) {
        printf("创建监控器失败\n");
        return;
    }
    
    printf("批量添加监控项:\n");
    const char *paths[] = {
        "/tmp/file1.txt",
        "/tmp/file2.txt",
        "/tmp/file3.txt",
        "/etc/hosts",
        "/var/log/syslog"
    };
    int num_paths = sizeof(paths) / sizeof(paths[0]);
    
    for (int i = 0; i < num_paths; i++) {
        printf("  添加: %s\n", paths[i]);
        if (!file_watcher_add(fw, paths[i], &error)) {
            printf("    (文件可能不存在)\n");
        }
    }
    
    printf("\n批量移除监控项:\n");
    for (int i = 0; i < num_paths; i++) {
        printf("  移除: %s\n", paths[i]);
        file_watcher_remove(fw, paths[i], &error);
    }
    
    file_watcher_free(fw);
}

// 演示 4: 监控策略
static void demo_strategies(void) {
    printf("\n=== 演示 4: 监控策略 ===\n");
    
    printf("1. 轮询模式 (Polling)\n");
    printf("   优点: 跨平台兼容性好\n");
    printf("   缺点: CPU 占用较高\n");
    printf("   适用: 所有平台\n");
    printf("\n");
    
    printf("2. 事件驱动模式 (inotify/kqueue/FSEvents)\n");
    printf("   优点: 实时性好，资源占用低\n");
    printf("   缺点: 平台相关\n");
    printf("   适用: Linux/macOS/Windows\n");
    printf("\n");
    
    printf("3. 混合模式\n");
    printf("   优先使用事件驱动\n");
    printf("   回退到轮询模式\n");
    printf("   平衡性能和兼容性\n");
}

// 演示 5: 实际应用
static void demo_applications(void) {
    printf("\n=== 演示 5: 实际应用场景 ===\n");
    
    printf("1. 配置文件热重载\n");
    printf("   - 监控配置文件变化\n");
    printf("   - 自动重新加载配置\n");
    printf("   - 无需重启服务\n");
    printf("\n");
    
    printf("2. 日志文件监控\n");
    printf("   - 实时监控日志输出\n");
    printf("   - 日志轮转检测\n");
    printf("   - 异常日志告警\n");
    printf("\n");
    
    printf("3. 文件同步\n");
    printf("   - 检测文件变化\n");
    printf("   - 触发同步操作\n");
    printf("   - 增量备份\n");
    printf("\n");
    
    printf("4. 开发工具\n");
    printf("   - 代码变更检测\n");
    printf("   - 自动编译/重启\n");
    printf("   - 实时重载浏览器\n");
    printf("\n");
    
    printf("5. 安全监控\n");
    printf("   - 监控敏感文件\n");
    printf("   - 检测未授权修改\n");
    printf("   - 文件完整性检查\n");
}

// 演示 6: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 6: 最佳实践 ===\n");
    
    printf("1. 错误处理\n");
    printf("   - 检查所有 API 返回值\n");
    printf("   - 优雅处理文件不存在\n");
    printf("   - 记录监控错误日志\n");
    printf("\n");
    
    printf("2. 性能优化\n");
    printf("   - 限制监控文件数量\n");
    printf("   - 使用合适的轮询间隔\n");
    printf("   - 避免递归监控大目录\n");
    printf("\n");
    
    printf("3. 资源管理\n");
    printf("   - 及时释放监控器\n");
    printf("   - 移除不需要的监控项\n");
    printf("   - 避免内存泄漏\n");
    printf("\n");
    
    printf("4. 线程安全\n");
    printf("   - 回调函数保持简短\n");
    printf("   - 避免在回调中阻塞\n");
    printf("   - 使用队列处理事件\n");
}

int main(void) {
    printf("========================================\n");
    printf("    文件监控演示程序\n");
    printf("========================================\n");
    
    demo_basic();
    demo_event_types();
    demo_batch_watch();
    demo_strategies();
    demo_applications();
    demo_best_practices();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
