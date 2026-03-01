/**
 * 共享内存演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/shm.h"

static void demo_basic_create(void) {
    printf("\n=== 演示 1: 创建共享内存 ===\n");

    const char *name = "/test_shm";
    size_t size = 1024;

    printf("创建共享内存: name=%s, size=%zu\n", name, size);

    void *ptr = shm_open_map(name, size);
    if (!ptr) {
        printf("创建失败\n");
        return;
    }

    printf("创建成功! ptr=%p\n", ptr);

    memset(ptr, 0, size);
    strcpy((char *)ptr, "Hello from shared memory!");
    printf("写入数据: %s\n", (char *)ptr);

    shm_close_unmap(ptr, name, size);
    printf("已关闭并删除共享内存\n");
}

static void demo_basic_open(void) {
    printf("\n=== 演示 2: 打开共享内存 ===\n");

    const char *name = "/test_shm2";
    size_t size = 512;

    printf("创建共享内存: name=%s, size=%zu\n", name, size);

    void *ptr = shm_open_map(name, size);
    if (!ptr) {
        printf("创建失败\n");
        return;
    }

    memset(ptr, 0, size);
    strcpy((char *)ptr, "Test data for sharing");
    printf("写入数据: %s\n", (char *)ptr);

    printf("\n模拟另一个进程打开...\n");
    void *ptr2 = shm_open_map(name, size);
    if (ptr2) {
        printf("打开成功! ptr2=%p\n", ptr2);
        printf("读取数据: %s\n", (char *)ptr2);
        shm_close_unmap(ptr2, name, size);
    }

    shm_close_unmap(ptr, name, size);
    printf("原进程已关闭\n");
}

static void demo_config(void) {
    printf("\n=== 演示 3: 配置选项 ===\n");

    const char *name = "/test_shm3";
    size_t size = 256;

    printf("使用配置创建共享内存...\n");

    shm_config_t config = {
        .create_if_not_exists = true,
        .exclusive = false,
        .unlink_on_close = true,
        .read_only = false,
        .permissions = 0644,
        .min_size = 0,
        .max_size = 0
    };

    printf("配置:\n");
    printf("  create_if_not_exists: %s\n", config.create_if_not_exists ? "是" : "否");
    printf("  exclusive: %s\n", config.exclusive ? "是" : "否");
    printf("  unlink_on_close: %s\n", config.unlink_on_close ? "是" : "否");
    printf("  permissions: 0%o\n", config.permissions);

    void *ptr = shm_open_map_ex(name, size, &config, NULL);
    if (!ptr) {
        printf("创建失败\n");
        return;
    }

    printf("创建成功! ptr=%p\n", ptr);

    shm_close_unmap(ptr, name, size);
    printf("已关闭\n");
}

static void demo_state(void) {
    printf("\n=== 演示 4: 状态查询 ===\n");

    const char *name = "/test_shm4";
    size_t size = 128;

    printf("创建共享内存并查询状态...\n");

    shm_state_t state;
    memset(&state, 0, sizeof(state));

    void *ptr = shm_open_map_ex(name, size, NULL, &state);
    if (!ptr) {
        printf("创建失败\n");
        return;
    }

    printf("状态:\n");
    printf("  last_error: %d\n", state.last_error);
    printf("  error_code: %d\n", state.error_code);
    printf("  actual_size: %zu\n", state.actual_size);
    printf("  is_mapped: %s\n", state.is_mapped ? "是" : "否");
    printf("  is_read_only: %s\n", state.is_read_only ? "是" : "否");
    printf("  is_created: %s\n", state.is_created ? "是" : "否");

    shm_close_unmap(ptr, name, size);
}

static void demo_error_handling(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("测试无效参数:\n");
    void *ptr = shm_open_map_ex(NULL, 100, NULL, NULL);
    if (!ptr) {
        printf("  正确捕获空名称错误\n");
    }

    ptr = shm_open_map_ex("/test", 0, NULL, NULL);
    if (!ptr) {
        printf("  正确捕获零大小错误\n");
    }

    printf("\n测试正常操作:\n");
    shm_state_t state;
    memset(&state, 0, sizeof(state));
    void *ptr2 = shm_open_map_ex("/test_shm5", 64, NULL, &state);
    if (ptr2) {
        printf("  创建成功\n");
        printf("  last_error: %d (0=成功)\n", state.last_error);
        shm_close_unmap(ptr2, "/test_shm5", 64);
    }
}

static void demo_data_exchange(void) {
    printf("\n=== 演示 6: 数据交换 ===\n");

    const char *name = "/test_shm6";
    size_t size = 256;

    printf("模拟生产者写入数据...\n");

    void *ptr = shm_open_map(name, size);
    if (!ptr) {
        printf("创建失败\n");
        return;
    }

    int *counter = (int *)ptr;
    *counter = 0;

    for (int i = 0; i < 5; i++) {
        (*counter)++;
        printf("  写入: counter = %d\n", *counter);
        sleep(1);
    }

    shm_close_unmap(ptr, name, size);
    printf("生产者完成\n");
}

int main(void) {
    printf("========================================\n");
    printf("    共享内存演示\n");
    printf("========================================\n");

    demo_basic_create();
    demo_basic_open();
    demo_config();
    demo_state();
    demo_error_handling();
    demo_data_exchange();

    printf("\n========================================\n");
    printf("演示完成!\n");
    printf("========================================\n");

    return 0;
}
