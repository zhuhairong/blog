/**
 * 键值存储演示程序
 *
 * 功能：
 * - 键值对存储和读取
 * - 批量操作
 * - 持久化存储
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/kv_store.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    const char *filename = "/tmp/demo_kv.db";

    printf("使用文件: %s\n\n", filename);

    // 保存键值对
    printf("保存键值对:\n");
    kv_save(filename, "name", "Alice");
    kv_save(filename, "age", "30");
    kv_save(filename, "city", "Beijing");
    printf("  name = Alice\n");
    printf("  age = 30\n");
    printf("  city = Beijing\n");

    // 读取键值对
    printf("\n读取键值对:\n");
    char *name = kv_load(filename, "name");
    char *age = kv_load(filename, "age");
    char *city = kv_load(filename, "city");

    if (name) printf("  name = %s\n", name);
    if (age) printf("  age = %s\n", age);
    if (city) printf("  city = %s\n", city);

    free(name);
    free(age);
    free(city);
}

// 演示 2: 键存在检查
static void demo_exists(void) {
    printf("\n=== 演示 2: 键存在检查 ===\n");

    const char *filename = "/tmp/demo_kv.db";

    printf("检查键是否存在:\n");
    printf("  'name' 存在: %s\n", kv_exists(filename, "name") ? "是" : "否");
    printf("  'email' 存在: %s\n", kv_exists(filename, "email") ? "是" : "否");
}

// 演示 3: 删除操作
static void demo_delete(void) {
    printf("\n=== 演示 3: 删除操作 ===\n");

    const char *filename = "/tmp/demo_kv2.db";

    // 先创建一些数据
    kv_save(filename, "key1", "value1");
    kv_save(filename, "key2", "value2");
    kv_save(filename, "key3", "value3");

    printf("初始键值对:\n");
    printf("  key1, key2, key3\n");

    printf("\n删除 'key2':\n");
    kv_error_t error = kv_delete(filename, "key2");
    printf("  结果: %s\n", error == KV_OK ? "成功" : "失败");

    printf("\n删除后检查:\n");
    printf("  'key1' 存在: %s\n", kv_exists(filename, "key1") ? "是" : "否");
    printf("  'key2' 存在: %s\n", kv_exists(filename, "key2") ? "是" : "否");
    printf("  'key3' 存在: %s\n", kv_exists(filename, "key3") ? "是" : "否");
}

// 演示 4: 批量操作
static void demo_batch(void) {
    printf("\n=== 演示 4: 批量操作 ===\n");

    const char *filename = "/tmp/demo_kv3.db";

    kv_entry_t entries[] = {
        {"host", "localhost"},
        {"port", "8080"},
        {"debug", "true"},
        {"timeout", "30"}
    };
    int count = sizeof(entries) / sizeof(entries[0]);

    printf("批量保存 %d 个键值对:\n", count);
    for (int i = 0; i < count; i++) {
        printf("  %s = %s\n", entries[i].key, entries[i].value);
    }

    kv_error_t error = kv_save_batch(filename, entries, count);
    printf("\n保存结果: %s\n", error == KV_OK ? "成功" : "失败");

    // 读取所有
    kv_entry_t *loaded_entries;
    size_t loaded_count;
    error = kv_get_all(filename, &loaded_entries, &error);

    if (error == KV_OK) {
        printf("\n读取到 %zu 个键值对:\n", loaded_count);
        for (size_t i = 0; i < loaded_count; i++) {
            printf("  %s = %s\n", loaded_entries[i].key, loaded_entries[i].value);
        }
        kv_free_entries(loaded_entries, loaded_count);
    }
}

// 演示 5: 统计信息
static void demo_stats(void) {
    printf("\n=== 演示 5: 统计信息 ===\n");

    const char *filename = "/tmp/demo_kv4.db";

    // 创建一些数据
    kv_save(filename, "a", "1");
    kv_save(filename, "b", "2");
    kv_save(filename, "c", "3");

    size_t entry_count, file_size;
    kv_error_t error = kv_get_stats(filename, &entry_count, &file_size);

    if (error == KV_OK) {
        printf("存储统计:\n");
        printf("  条目数: %zu\n", entry_count);
        printf("  文件大小: %zu 字节\n", file_size);
    }
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 应用配置\n");
    printf("   - 用户偏好设置\n");
    printf("   - 应用状态保存\n");
    printf("\n");

    printf("2. 会话管理\n");
    printf("   - 用户会话数据\n");
    printf("   - 临时状态存储\n");
    printf("\n");

    printf("3. 缓存系统\n");
    printf("   - 简单数据缓存\n");
    printf("   - 配置缓存\n");
    printf("\n");

    printf("4. 嵌入式系统\n");
    printf("   - 设备配置\n");
    printf("   - 参数存储\n");
}

int main(void) {
    printf("========================================\n");
    printf("    键值存储演示\n");
    printf("========================================\n");

    demo_basic();
    demo_exists();
    demo_delete();
    demo_batch();
    demo_stats();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
