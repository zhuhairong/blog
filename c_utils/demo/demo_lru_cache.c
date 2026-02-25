/**
 * LRU 缓存演示程序
 *
 * 功能：
 * - 缓存存取操作
 * - 缓存淘汰策略
 * - 统计信息
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/lru_cache.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    printf("创建容量为 3 的 LRU 缓存...\n");
    lru_cache_t *cache = lru_cache_create(3);

    printf("\n添加键值对:\n");
    int *v1 = malloc(sizeof(int)); *v1 = 100;
    int *v2 = malloc(sizeof(int)); *v2 = 200;
    int *v3 = malloc(sizeof(int)); *v3 = 300;

    lru_cache_put(cache, "key1", v1);
    lru_cache_put(cache, "key2", v2);
    lru_cache_put(cache, "key3", v3);

    printf("  key1 = %d\n", *(int*)lru_cache_get(cache, "key1"));
    printf("  key2 = %d\n", *(int*)lru_cache_get(cache, "key2"));
    printf("  key3 = %d\n", *(int*)lru_cache_get(cache, "key3"));

    printf("\n缓存大小: %zu/%zu\n", lru_cache_size(cache), lru_cache_capacity(cache));

    // 清理
    lru_cache_free(cache);
    free(v1); free(v2); free(v3);
}

// 演示 2: LRU 淘汰策略
static void demo_eviction(void) {
    printf("\n=== 演示 2: LRU 淘汰策略 ===\n");

    printf("创建容量为 2 的缓存...\n");
    lru_cache_t *cache = lru_cache_create(2);

    int *v1 = malloc(sizeof(int)); *v1 = 1;
    int *v2 = malloc(sizeof(int)); *v2 = 2;
    int *v3 = malloc(sizeof(int)); *v3 = 3;

    printf("\n添加 key1=1, key2=2\n");
    lru_cache_put(cache, "key1", v1);
    lru_cache_put(cache, "key2", v2);

    printf("访问 key1 (使其变为最近使用)...\n");
    void *val = lru_cache_get(cache, "key1");
    if (val) printf("  key1 = %d\n", *(int*)val);

    printf("\n添加 key3=3 (应该淘汰 key2)...\n");
    lru_cache_put(cache, "key3", v3);

    printf("检查缓存内容:\n");
    printf("  key1: %s\n", lru_cache_contains(cache, "key1") ? "存在" : "不存在");
    printf("  key2: %s (被淘汰)\n", lru_cache_contains(cache, "key2") ? "存在" : "不存在");
    printf("  key3: %s\n", lru_cache_contains(cache, "key3") ? "存在" : "不存在");

    // 清理
    lru_cache_free(cache);
    free(v1); free(v2); free(v3);
}

// 演示 3: 缓存统计
static void demo_stats(void) {
    printf("\n=== 演示 3: 缓存统计 ===\n");

    lru_cache_t *cache = lru_cache_create(3);

    int *v1 = malloc(sizeof(int)); *v1 = 1;
    int *v2 = malloc(sizeof(int)); *v2 = 2;

    printf("执行操作序列:\n");
    printf("  put(key1, 1)\n");
    lru_cache_put(cache, "key1", v1);

    printf("  get(key1) - 命中\n");
    lru_cache_get(cache, "key1");

    printf("  get(key2) - 未命中\n");
    lru_cache_get(cache, "key2");

    printf("  get(key1) - 命中\n");
    lru_cache_get(cache, "key1");

    lru_cache_stats_t stats;
    lru_cache_error_t error = lru_cache_get_stats(cache, &stats);

    if (error == LRU_CACHE_OK) {
        printf("\n缓存统计:\n");
        printf("  命中次数: %zu\n", stats.hits);
        printf("  未命中次数: %zu\n", stats.misses);
        printf("  写入次数: %zu\n", stats.puts);
        printf("  淘汰次数: %zu\n", stats.evictions);
        printf("  当前大小: %zu\n", stats.current_size);
        printf("  容量: %zu\n", stats.capacity);
        printf("  命中率: %.2f%%\n", stats.hit_rate * 100);
    }

    // 清理
    lru_cache_free(cache);
    free(v1); free(v2);
}

// 演示 4: 删除和清空
static void demo_remove_clear(void) {
    printf("\n=== 演示 4: 删除和清空 ===\n");

    lru_cache_t *cache = lru_cache_create(5);

    int *v1 = malloc(sizeof(int)); *v1 = 1;
    int *v2 = malloc(sizeof(int)); *v2 = 2;
    int *v3 = malloc(sizeof(int)); *v3 = 3;

    lru_cache_put(cache, "a", v1);
    lru_cache_put(cache, "b", v2);
    lru_cache_put(cache, "c", v3);

    printf("初始缓存: a, b, c\n");
    printf("  大小: %zu\n", lru_cache_size(cache));

    printf("\n删除键 'b'...\n");
    lru_cache_remove(cache, "b");
    printf("  删除后大小: %zu\n", lru_cache_size(cache));
    printf("  'b' 存在: %s\n", lru_cache_contains(cache, "b") ? "是" : "否");

    printf("\n清空缓存...\n");
    lru_cache_clear(cache);
    printf("  清空后大小: %zu\n", lru_cache_size(cache));

    // 清理
    lru_cache_free(cache);
    free(v1); free(v2); free(v3);
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 数据库查询缓存\n");
    printf("   - 缓存热点查询结果\n");
    printf("   - 减少数据库压力\n");
    printf("\n");

    printf("2. Web 页面缓存\n");
    printf("   - 缓存频繁访问的页面\n");
    printf("   - 提高响应速度\n");
    printf("\n");

    printf("3. 文件系统缓存\n");
    printf("   - 缓存文件元数据\n");
    printf("   - 加速文件访问\n");
    printf("\n");

    printf("4. DNS 缓存\n");
    printf("   - 缓存域名解析结果\n");
    printf("   - 减少网络请求\n");
}

int main(void) {
    printf("========================================\n");
    printf("    LRU 缓存演示\n");
    printf("========================================\n");

    demo_basic();
    demo_eviction();
    demo_stats();
    demo_remove_clear();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
