/**
 * @file demo_consistent_hash.c
 * @brief 一致性哈希演示
 *
 * 本演示展示了一致性哈希算法的实现和应用，包括：
 * - 节点添加和删除
 * - 键值映射
 * - 虚拟节点（副本）
 * - 节点故障处理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/consistent_hash.h"

// 演示 1: 基本用法
static void demo_basic_usage(void) {
    printf("\n=== 演示 1: 基本用法 ===\n");

    consistent_hash_t ch;
    ch_error_t err = ch_init(&ch, NULL);
    if (err != CH_OK) {
        printf("初始化失败，错误码: %d\n", err);
        return;
    }

    // 添加节点
    printf("添加节点...\n");
    ch_add_node(&ch, 1, 100);  // 节点1，100个副本
    ch_add_node(&ch, 2, 100);  // 节点2，100个副本
    ch_add_node(&ch, 3, 100);  // 节点3，100个副本

    printf("节点数量: %zu\n", ch_get_node_count(&ch));

    // 映射键到节点
    const char* keys[] = {"user:1001", "user:1002", "user:1003", "user:1004", "user:1005"};
    printf("\n键值映射:\n");
    for (int i = 0; i < 5; i++) {
        int node = ch_get_node_str(&ch, keys[i]);
        printf("  %s -> 节点 %d\n", keys[i], node);
    }

    ch_free(&ch);
}

// 演示 2: 虚拟节点
static void demo_virtual_nodes(void) {
    printf("\n=== 演示 2: 虚拟节点 ===\n");

    consistent_hash_t ch;
    ch_init(&ch, NULL);

    // 添加不同副本数量的节点
    printf("添加不同副本数量的节点:\n");
    ch_add_node(&ch, 1, 50);   // 节点1: 50个副本
    ch_add_node(&ch, 2, 100);  // 节点2: 100个副本
    ch_add_node(&ch, 3, 150);  // 节点3: 150个副本

    printf("  节点1副本数: %zu\n", ch_get_replica_count(&ch, 1));
    printf("  节点2副本数: %zu\n", ch_get_replica_count(&ch, 2));
    printf("  节点3副本数: %zu\n", ch_get_replica_count(&ch, 3));

    // 统计键分布
    int node1_count = 0, node2_count = 0, node3_count = 0;
    for (int i = 0; i < 1000; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        int node = ch_get_node_str(&ch, key);
        if (node == 1) node1_count++;
        else if (node == 2) node2_count++;
        else if (node == 3) node3_count++;
    }

    printf("\n键分布统计 (1000个键):\n");
    printf("  节点1: %d (%.1f%%)\n", node1_count, node1_count / 10.0);
    printf("  节点2: %d (%.1f%%)\n", node2_count, node2_count / 10.0);
    printf("  节点3: %d (%.1f%%)\n", node3_count, node3_count / 10.0);

    ch_free(&ch);
}

// 演示 3: 节点添加的影响
static void demo_add_node(void) {
    printf("\n=== 演示 3: 节点添加的影响 ===\n");

    consistent_hash_t ch;
    ch_init(&ch, NULL);

    // 初始两个节点
    ch_add_node(&ch, 1, 100);
    ch_add_node(&ch, 2, 100);

    // 记录初始映射
    int initial_mapping[10];
    printf("初始映射 (2个节点):\n");
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        initial_mapping[i] = ch_get_node_str(&ch, key);
        printf("  %s -> 节点 %d\n", key, initial_mapping[i]);
    }

    // 添加第三个节点
    printf("\n添加节点3...\n");
    ch_add_node(&ch, 3, 100);

    // 统计变化
    int changed = 0;
    printf("\n新映射 (3个节点):\n");
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        int new_node = ch_get_node_str(&ch, key);
        const char* status = (new_node != initial_mapping[i]) ? " (已迁移)" : "";
        printf("  %s -> 节点 %d%s\n", key, new_node, status);
        if (new_node != initial_mapping[i]) changed++;
    }

    printf("\n键迁移率: %d/%d (%.1f%%)\n", changed, 10, changed * 10.0);

    ch_free(&ch);
}

// 演示 4: 节点删除
static void demo_remove_node(void) {
    printf("\n=== 演示 4: 节点删除 ===\n");

    consistent_hash_t ch;
    ch_init(&ch, NULL);

    ch_add_node(&ch, 1, 100);
    ch_add_node(&ch, 2, 100);
    ch_add_node(&ch, 3, 100);

    printf("初始节点数量: %zu\n", ch_get_node_count(&ch));

    // 记录初始映射
    int initial_mapping[10];
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        initial_mapping[i] = ch_get_node_str(&ch, key);
    }

    // 删除节点2
    printf("\n删除节点2...\n");
    ch_error_t err = ch_remove_node(&ch, 2);
    if (err == CH_OK) {
        printf("删除成功\n");
    } else {
        printf("删除失败，错误码: %d\n", err);
    }

    printf("当前节点数量: %zu\n", ch_get_node_count(&ch));

    // 显示迁移情况
    printf("\n键迁移情况:\n");
    for (int i = 0; i < 10; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        int new_node = ch_get_node_str(&ch, key);
        if (initial_mapping[i] == 2) {
            printf("  %s: 节点2 -> 节点%d (已迁移)\n", key, new_node);
        } else {
            printf("  %s: 节点%d (未变化)\n", key, new_node);
        }
    }

    ch_free(&ch);
}

// 演示 5: 节点存在性检查
static void demo_node_exists(void) {
    printf("\n=== 演示 5: 节点存在性检查 ===\n");

    consistent_hash_t ch;
    ch_init(&ch, NULL);

    ch_add_node(&ch, 1, 100);
    ch_add_node(&ch, 2, 100);

    printf("节点存在性检查:\n");
    printf("  节点1存在: %s\n", ch_node_exists(&ch, 1) ? "是" : "否");
    printf("  节点2存在: %s\n", ch_node_exists(&ch, 2) ? "是" : "否");
    printf("  节点3存在: %s\n", ch_node_exists(&ch, 3) ? "是" : "否");

    ch_free(&ch);
}

// 演示 6: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CH_OK (%d): 成功\n", CH_OK);
    printf("  CH_ERROR_INVALID_NODE (%d): 无效节点\n", CH_ERROR_INVALID_NODE);
    printf("  CH_ERROR_INVALID_REPLICAS (%d): 无效副本数\n", CH_ERROR_INVALID_REPLICAS);
    printf("  CH_ERROR_INVALID_KEY (%d): 无效键\n", CH_ERROR_INVALID_KEY);
    printf("  CH_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CH_ERROR_MEMORY_ALLOC);
    printf("  CH_ERROR_NODE_NOT_FOUND (%d): 节点未找到\n", CH_ERROR_NODE_NOT_FOUND);

    consistent_hash_t ch;
    ch_init(&ch, NULL);

    // 测试无效操作
    printf("\n测试无效操作:\n");
    ch_error_t err = ch_add_node(&ch, -1, 100);
    printf("  添加无效节点ID: 错误码 %d\n", err);

    err = ch_add_node(&ch, 1, 0);
    printf("  添加0副本节点: 错误码 %d\n", err);

    err = ch_remove_node(&ch, 999);
    printf("  删除不存在的节点: 错误码 %d\n", err);

    ch_free(&ch);
}

// 演示 7: 负载均衡
static void demo_load_balance(void) {
    printf("\n=== 演示 7: 负载均衡 ===\n");

    consistent_hash_t ch;
    ch_init(&ch, NULL);

    // 添加3个节点，相同副本数
    ch_add_node(&ch, 1, 100);
    ch_add_node(&ch, 2, 100);
    ch_add_node(&ch, 3, 100);

    // 模拟10000个请求
    int node1_count = 0, node2_count = 0, node3_count = 0;
    for (int i = 0; i < 10000; i++) {
        char key[64];
        snprintf(key, sizeof(key), "request_%d_user_%d", i, i % 1000);
        int node = ch_get_node_str(&ch, key);
        if (node == 1) node1_count++;
        else if (node == 2) node2_count++;
        else if (node == 3) node3_count++;
    }

    printf("请求分布 (10000个请求):\n");
    printf("  节点1: %d (%.2f%%)\n", node1_count, node1_count / 100.0);
    printf("  节点2: %d (%.2f%%)\n", node2_count, node2_count / 100.0);
    printf("  节点3: %d (%.2f%%)\n", node3_count, node3_count / 100.0);

    double avg = 10000.0 / 3;
    double variance = ((node1_count - avg) * (node1_count - avg) +
                       (node2_count - avg) * (node2_count - avg) +
                       (node3_count - avg) * (node3_count - avg)) / 3;
    printf("\n负载均衡度 (标准差): %.2f\n", sqrt(variance));

    ch_free(&ch);
}

// 演示 8: 节点故障模拟
static void demo_node_failure(void) {
    printf("\n=== 演示 8: 节点故障模拟 ===\n");

    consistent_hash_t ch;
    ch_init(&ch, NULL);

    ch_add_node(&ch, 1, 100);
    ch_add_node(&ch, 2, 100);
    ch_add_node(&ch, 3, 100);

    // 记录初始映射
    int node1_count = 0, node2_count = 0, node3_count = 0;
    for (int i = 0; i < 1000; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        int node = ch_get_node_str(&ch, key);
        if (node == 1) node1_count++;
        else if (node == 2) node2_count++;
        else if (node == 3) node3_count++;
    }

    printf("故障前分布:\n");
    printf("  节点1: %d (%.1f%%)\n", node1_count, node1_count / 10.0);
    printf("  节点2: %d (%.1f%%)\n", node2_count, node2_count / 10.0);
    printf("  节点3: %d (%.1f%%)\n", node3_count, node3_count / 10.0);

    // 模拟节点2故障
    printf("\n模拟节点2故障...\n");
    ch_remove_node(&ch, 2);

    // 重新统计
    int new_node1 = 0, new_node3 = 0;
    int migrated = 0;
    for (int i = 0; i < 1000; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d", i);
        int node = ch_get_node_str(&ch, key);
        if (node == 1) new_node1++;
        else if (node == 3) new_node3++;

        // 检查是否迁移
        if ((node == 1 && i % 3 != 0) ||
            (node == 3 && i % 3 != 2)) {
            migrated++;
        }
    }

    printf("\n故障后分布:\n");
    printf("  节点1: %d (%.1f%%)\n", new_node1, new_node1 / 10.0);
    printf("  节点3: %d (%.1f%%)\n", new_node3, new_node3 / 10.0);
    printf("\n需要迁移的键: %d (%.1f%%)\n", migrated, migrated / 10.0);

    ch_free(&ch);
}

// 演示 9: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 9: 应用场景 ===\n");

    printf("一致性哈希的应用场景:\n\n");

    printf("1. 分布式缓存:\n");
    printf("   - Memcached/Redis 集群\n");
    printf("   - 缓存键均匀分布到多个节点\n");
    printf("   - 节点增减时最小化缓存失效\n\n");

    printf("2. 分布式数据库:\n");
    printf("   - 数据分片 (Sharding)\n");
    printf("   - 水平扩展时数据迁移最小化\n");
    printf("   - 负载均衡\n\n");

    printf("3. 负载均衡:\n");
    printf("   - 请求路由到后端服务器\n");
    printf("   - 服务器故障时自动切换\n");
    printf("   - 会话保持\n\n");

    printf("4. P2P网络:\n");
    printf("   - 资源定位\n");
    printf("   - 节点发现\n");
    printf("   - 数据分布\n");

    // 实际示例：缓存服务器
    printf("\n实际示例: 缓存服务器集群\n");
    consistent_hash_t ch;
    ch_init(&ch, NULL);

    ch_add_node(&ch, 1, 150);  // cache-server-1
    ch_add_node(&ch, 2, 150);  // cache-server-2
    ch_add_node(&ch, 3, 150);  // cache-server-3

    const char* cache_keys[] = {
        "user:session:1001",
        "user:profile:1002",
        "product:info:5001",
        "order:details:8001",
        "inventory:stock:3001"
    };

    printf("\n缓存键分布:\n");
    for (int i = 0; i < 5; i++) {
        int node = ch_get_node_str(&ch, cache_keys[i]);
        printf("  %s -> cache-server-%d\n", cache_keys[i], node);
    }

    ch_free(&ch);
}

// 演示 10: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 10: 最佳实践 ===\n");

    printf("一致性哈希最佳实践:\n\n");

    printf("1. 虚拟节点数量:\n");
    printf("   - 每个物理节点 100-200 个虚拟节点\n");
    printf("   - 节点数少时增加虚拟节点数\n");
    printf("   - 平衡内存使用和负载均衡度\n\n");

    printf("2. 哈希函数选择:\n");
    printf("   - 使用高质量的哈希函数 (MurmurHash, FNV)\n");
    printf("   - 避免哈希冲突\n");
    printf("   - 均匀分布\n\n");

    printf("3. 节点管理:\n");
    printf("   - 平滑扩缩容\n");
    printf("   - 预热新节点\n");
    printf("   - 优雅下线旧节点\n\n");

    printf("4. 监控和调优:\n");
    printf("   - 监控节点负载\n");
    printf("   - 调整虚拟节点比例\n");
    printf("   - 热点数据处理\n");
}

int main(void) {
    printf("========================================\n");
    printf("    一致性哈希演示程序\n");
    printf("========================================\n");

    demo_basic_usage();
    demo_virtual_nodes();
    demo_add_node();
    demo_remove_node();
    demo_node_exists();
    demo_error_handling();
    demo_load_balance();
    demo_node_failure();
    demo_use_cases();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
