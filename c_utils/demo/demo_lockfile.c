/**
 * 锁文件演示程序
 *
 * 功能：
 * - 文件锁定和解锁
 * - 阻塞和非阻塞锁定
 * - 锁状态检查
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../c_utils/lockfile.h"

// 演示 1: 基本锁定
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本锁定 ===\n");

    const char *lockfile = "/tmp/demo_lockfile.lock";

    printf("锁定文件: %s\n", lockfile);

    int fd = lockfile_lock(lockfile);
    if (fd >= 0) {
        printf("锁定成功! (fd=%d)\n", fd);
        printf("模拟执行一些操作...\n");
        sleep(1);

        lockfile_unlock(fd);
        printf("已解锁\n");
    } else {
        printf("锁定失败\n");
    }
}

// 演示 2: 非阻塞锁定
static void demo_non_blocking(void) {
    printf("\n=== 演示 2: 非阻塞锁定 ===\n");

    const char *lockfile = "/tmp/demo_lockfile2.lock";

    printf("第一次锁定...\n");
    int fd1 = lockfile_lock(lockfile);
    if (fd1 >= 0) {
        printf("第一次锁定成功 (fd=%d)\n", fd1);

        printf("\n尝试第二次非阻塞锁定...\n");
        lockfile_error_t error;
        int fd2 = lockfile_try_lock(lockfile, &error);

        if (fd2 < 0) {
            printf("第二次锁定失败 (预期): ");
            switch (error) {
                case LOCKFILE_ALREADY_LOCKED:
                    printf("文件已被锁定\n");
                    break;
                default:
                    printf("错误码 %d\n", error);
            }
        }

        lockfile_unlock(fd1);
        printf("\n已释放第一次锁定\n");
    }
}

// 演示 3: 锁状态检查
static void demo_status_check(void) {
    printf("\n=== 演示 3: 锁状态检查 ===\n");

    const char *lockfile = "/tmp/demo_lockfile3.lock";

    lockfile_error_t error;

    printf("检查文件是否被锁定:\n");
    bool locked = lockfile_is_locked(lockfile, &error);
    printf("  锁定状态: %s\n", locked ? "是" : "否");

    printf("\n获取详细状态:\n");
    bool is_locked;
    pid_t owner;
    error = lockfile_get_status(lockfile, &is_locked, &owner);
    if (error == LOCKFILE_OK) {
        printf("  是否锁定: %s\n", is_locked ? "是" : "否");
        if (is_locked) {
            printf("  锁所有者 PID: %d\n", owner);
        }
    }

    printf("\n锁定文件...\n");
    int fd = lockfile_lock(lockfile);

    printf("再次检查状态:\n");
    locked = lockfile_is_locked(lockfile, &error);
    printf("  锁定状态: %s\n", locked ? "是" : "否");

    lockfile_unlock(fd);
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    lockfile_config_t config;
    lockfile_get_default_config(&config);

    printf("默认配置:\n");
    printf("  阻塞模式: %s\n", config.blocking ? "是" : "否");
    printf("  独占锁: %s\n", config.exclusive ? "是" : "否");
    printf("  非阻塞: %s\n", config.non_blocking ? "是" : "否");
    printf("  超时: %d ms\n", config.timeout_ms);
    printf("  创建文件: %s\n", config.create_file ? "是" : "否");
    printf("  文件权限: %o\n", config.file_mode);
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 单实例应用程序\n");
    printf("   - 防止程序多开\n");
    printf("   - 确保只有一个进程运行\n");
    printf("\n");

    printf("2. 资源保护\n");
    printf("   - 保护共享资源\n");
    printf("   - 防止并发访问冲突\n");
    printf("\n");

    printf("3. 任务调度\n");
    printf("   - 防止任务重叠执行\n");
    printf("   - cron 任务保护\n");
    printf("\n");

    printf("4. 数据库访问\n");
    printf("   - 防止并发写操作\n");
    printf("   - 事务保护\n");
}

int main(void) {
    printf("========================================\n");
    printf("    锁文件演示\n");
    printf("========================================\n");

    demo_basic();
    demo_non_blocking();
    demo_status_check();
    demo_config();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
