/**
 * 信号量演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../c_utils/sem.h"

static void demo_basic_create(void) {
    printf("\n=== 演示 1: 创建信号量 ===\n");

    const char *name = "/test_sem";
    unsigned int value = 1;

    printf("创建信号量: name=%s, value=%u\n", name, value);

    sem_handle_t sem = sem_create(name, value);
    if (!sem) {
        printf("创建失败\n");
        return;
    }

    printf("创建成功! sem=%p\n", sem);

    printf("P 操作 (等待)...\n");
    sem_p(sem);
    printf("P 操作成功\n");

    printf("V 操作 (释放)...\n");
    sem_v(sem);
    printf("V 操作成功\n");

    sem_close_delete(sem, name);
    printf("已关闭并删除信号量\n");
}

static void demo_binary(void) {
    printf("\n=== 演示 2: 二进制信号量 ===\n");

    const char *name = "/test_bin_sem";
    unsigned int value = 1;

    printf("创建二进制信号量 (初始值=1)...\n");

    sem_handle_t sem = sem_create(name, value);
    if (!sem) {
        printf("创建失败\n");
        return;
    }

    printf("模拟互斥访问:\n");

    pid_t pid = fork();
    if (pid == 0) {
        sem_handle_t child_sem = sem_open_ex(name, NULL);
        if (child_sem) {
            printf("子进程: 等待信号量...\n");
            sem_p(child_sem);
            printf("子进程: 获取信号量，开始工作...\n");
            sleep(1);
            printf("子进程: 完成工作，释放信号量\n");
            sem_v(child_sem);
            sem_close_delete(child_sem, name);
            exit(0);
        }
    }

    if (pid > 0) {
        printf("父进程: 等待信号量...\n");
        sem_p(sem);
        printf("父进程: 获取信号量，开始工作...\n");
        sleep(1);
        printf("父进程: 完成工作，释放信号量\n");
        sem_v(sem);
        wait(NULL);
    }

    sem_close_delete(sem, name);
    printf("演示完成\n");
}

static void demo_counting(void) {
    printf("\n=== 演示 3: 计数信号量 ===\n");

    const char *name = "/test_count_sem";
    unsigned int value = 3;

    printf("创建计数信号量 (初始值=3)...\n");

    sem_handle_t sem = sem_create(name, value);
    if (!sem) {
        printf("创建失败\n");
        return;
    }

    printf("模拟资源池 (3个资源):\n");

    for (int i = 0; i < 5; i++) {
        printf("  线程 %d: 等待资源...\n", i);
        sem_p(sem);
        printf("  线程 %d: 获取资源，使用中...\n", i);
        sleep(1);
        sem_v(sem);
        printf("  线程 %d: 释放资源\n", i);
    }

    sem_close_delete(sem, name);
    printf("演示完成\n");
}

static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    const char *name = "/test_config_sem";
    unsigned int value = 1;

    printf("使用配置创建信号量...\n");

    sem_config_t config = {
        .create_if_not_exists = true,
        .exclusive = false,
        .unlink_on_close = true,
        .max_value = 0
    };

    printf("配置:\n");
    printf("  create_if_not_exists: %s\n", config.create_if_not_exists ? "是" : "否");
    printf("  exclusive: %s\n", config.exclusive ? "是" : "否");
    printf("  unlink_on_close: %s\n", config.unlink_on_close ? "是" : "否");
    printf("  max_value: %u (0=无限制)\n", config.max_value);

    sem_state_t state;
    memset(&state, 0, sizeof(state));

    sem_handle_t sem = sem_create_ex(name, value, &config, &state);
    if (!sem) {
        printf("创建失败\n");
        return;
    }

    printf("状态:\n");
    printf("  last_error: %d\n", state.last_error);
    printf("  current_value: %u\n", state.current_value);
    printf("  is_initialized: %s\n", state.is_initialized ? "是" : "否");
    printf("  is_named: %s\n", state.is_named ? "是" : "否");

    sem_close_delete(sem, name);
    printf("演示完成\n");
}

static void demo_error_handling(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("测试正常操作:\n");
    sem_state_t state;
    memset(&state, 0, sizeof(state));

    sem_handle_t sem = sem_create_ex("/test_sem_err", 1, NULL, &state);
    if (sem) {
        printf("  创建成功\n");
        printf("  last_error: %d (0=成功)\n", state.last_error);
        printf("  current_value: %u\n", state.current_value);
        sem_close_delete(sem, "/test_sem_err");
    }
}

static void demo_state_query(void) {
    printf("\n=== 演示 6: 状态查询 ===\n");

    const char *name = "/test_state_sem";
    unsigned int value = 5;

    printf("创建信号量并查询状态...\n");

    sem_state_t state;
    memset(&state, 0, sizeof(state));

    sem_handle_t sem = sem_create_ex(name, value, NULL, &state);
    if (!sem) {
        printf("创建失败\n");
        return;
    }

    printf("初始状态:\n");
    printf("  last_error: %d\n", state.last_error);
    printf("  current_value: %u\n", state.current_value);
    printf("  is_initialized: %s\n", state.is_initialized ? "是" : "否");
    printf("  is_named: %s\n", state.is_named ? "是" : "否");

    printf("\nP 操作后查询:\n");
    sem_p(sem);

    int val;
    if (sem_getvalue(sem, &val) == 0) {
        printf("  当前值: %d\n", val);
    }

    sem_close_delete(sem, name);
    printf("演示完成\n");
}

int main(void) {
    printf("========================================\n");
    printf("    信号量演示\n");
    printf("========================================\n");

    demo_basic_create();
    demo_binary();
    demo_counting();
    demo_config();
    demo_error_handling();
    demo_state_query();

    printf("\n========================================\n");
    printf("演示完成!\n");
    printf("========================================\n");

    return 0;
}
