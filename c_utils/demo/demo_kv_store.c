/**
 * 键值存储演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/kv_store.h"

static void demo_basic_save_load(void) {
    printf("\n=== 演示 1: 基本保存和加载 ===\n");

    const char *filename = "/tmp/demo_kv_store.txt";

    printf("保存键值对:\n");
    kv_save(filename, "name", "demo_app");
    kv_save(filename, "version", "1.0.0");
    kv_save(filename, "debug", "true");
    kv_save(filename, "port", "8080");
    printf("  name = demo_app\n");
    printf("  version = 1.0.0\n");
    printf("  debug = true\n");
    printf("  port = 8080\n");

    printf("\n加载键值对:\n");
    char *name = kv_load(filename, "name");
    char *version = kv_load(filename, "version");
    char *debug = kv_load(filename, "debug");
    char *port = kv_load(filename, "port");
    char *missing = kv_load(filename, "missing");

    printf("  name = %s\n", name ? name : "(不存在)");
    printf("  version = %s\n", version ? version : "(不存在)");
    printf("  debug = %s\n", debug ? debug : "(不存在)");
    printf("  port = %s\n", port ? port : "(不存在)");
    printf("  missing = %s\n", missing ? missing : "(不存在)");

    free(name);
    free(version);
    free(debug);
    free(port);
    free(missing);

    remove(filename);
    printf("\n临时文件已删除\n");
}

static void demo_update_delete(void) {
    printf("\n=== 演示 2: 更新和删除 ===\n");

    const char *filename = "/tmp/demo_kv_update.txt";

    kv_save(filename, "count", "100");
    printf("设置 count = 100\n");
    char *val = kv_load(filename, "count");
    printf("获取: count = %s\n", val ? val : "(不存在)");
    free(val);

    kv_save(filename, "count", "200");
    printf("\n更新 count = 200\n");
    val = kv_load(filename, "count");
    printf("获取: count = %s\n", val ? val : "(不存在)");
    free(val);

    printf("\n删除 count...\n");
    kv_delete(filename, "count");
    val = kv_load(filename, "count");
    printf("获取: count = %s\n", val ? val : "(已删除)");
    free(val);

    remove(filename);
}

static void demo_exists(void) {
    printf("\n=== 演示 3: 键存在检查 ===\n");

    const char *filename = "/tmp/demo_kv_exists.txt";

    kv_save(filename, "key1", "value1");
    kv_save(filename, "key2", "value2");

    printf("检查键是否存在:\n");
    printf("  key1 存在: %s\n", kv_exists(filename, "key1") ? "是" : "否");
    printf("  key2 存在: %s\n", kv_exists(filename, "key2") ? "是" : "否");
    printf("  key3 存在: %s\n", kv_exists(filename, "key3") ? "是" : "否");

    remove(filename);
}

static void demo_get_all(void) {
    printf("\n=== 演示 4: 获取所有键值对 ===\n");

    const char *filename = "/tmp/demo_kv_all.txt";

    kv_save(filename, "a", "1");
    kv_save(filename, "b", "2");
    kv_save(filename, "c", "3");

    printf("所有键值对:\n");
    kv_entry_t *entries = NULL;
    kv_error_t error;
    size_t count = kv_get_all(filename, &entries, &error);

    if (error == KV_OK && entries) {
        for (size_t i = 0; i < count; i++) {
            printf("  %s = %s\n", entries[i].key, entries[i].value);
        }
        kv_free_entries(entries, count);
    }

    remove(filename);
}

static void demo_stats(void) {
    printf("\n=== 演示 5: 存储统计 ===\n");

    const char *filename = "/tmp/demo_kv_stats.txt";

    kv_save(filename, "key1", "value1");
    kv_save(filename, "key2", "value2");
    kv_save(filename, "key3", "value3");

    size_t entry_count, file_size;
    kv_error_t err = kv_get_stats(filename, &entry_count, &file_size);

    if (err == KV_OK) {
        printf("条目数量: %zu\n", entry_count);
        printf("文件大小: %zu 字节\n", file_size);
    }

    remove(filename);
}

int main(void) {
    printf("========================================\n");
    printf("    键值存储演示\n");
    printf("========================================\n");

    demo_basic_save_load();
    demo_update_delete();
    demo_exists();
    demo_get_all();
    demo_stats();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
