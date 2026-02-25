/**
 * @file demo_bloom.c
 * @brief 布隆过滤器演示
 *
 * 本演示展示了布隆过滤器的各种用法，包括：
 * - 创建和销毁
 * - 添加和检查元素
 * - 假阳性率测试
 * - 序列化
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/bloom.h"

// 演示 1: 基本操作
static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    // 创建布隆过滤器：预期 1000 个元素，1% 假阳性率
    bloom_t* bloom = bloom_create(1000, 0.01);
    if (!bloom) {
        fprintf(stderr, "创建布隆过滤器失败\n");
        return;
    }

    printf("创建布隆过滤器:\n");
    printf("  预期元素数: 1000\n");
    printf("  目标假阳性率: 1%%\n");

    // 添加元素
    const char* elements[] = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon"
    };
    int num_elements = sizeof(elements) / sizeof(elements[0]);

    printf("\n添加 %d 个元素:\n", num_elements);
    for (int i = 0; i < num_elements; i++) {
        if (bloom_add(bloom, elements[i], strlen(elements[i]))) {
            printf("  + \"%s\"\n", elements[i]);
        }
    }

    // 检查元素
    printf("\n检查已添加的元素:\n");
    for (int i = 0; i < num_elements; i++) {
        bool exists = bloom_check(bloom, elements[i], strlen(elements[i]));
        printf("  \"%s\": %s\n", elements[i], exists ? "可能存在" : "肯定不存在");
    }

    // 检查未添加的元素
    printf("\n检查未添加的元素:\n");
    const char* not_added[] = {"mango", "orange", "peach", "plum", "raspberry"};
    int num_not_added = sizeof(not_added) / sizeof(not_added[0]);
    for (int i = 0; i < num_not_added; i++) {
        bool exists = bloom_check(bloom, not_added[i], strlen(not_added[i]));
        printf("  \"%s\": %s\n", not_added[i], exists ? "可能存在 (假阳性!)" : "肯定不存在");
    }

    bloom_free(bloom);
}

// 演示 2: 假阳性率测试
static void demo_false_positive_rate(void) {
    printf("\n=== 演示 2: 假阳性率测试 ===\n");

    // 测试不同假阳性率
    double fp_rates[] = {0.1, 0.01, 0.001, 0.0001};
    int num_rates = sizeof(fp_rates) / sizeof(fp_rates[0]);

    for (int r = 0; r < num_rates; r++) {
        double target_fp = fp_rates[r];
        bloom_t* bloom = bloom_create(1000, target_fp);
        if (!bloom) continue;

        // 添加 1000 个元素
        char buffer[32];
        for (int i = 0; i < 1000; i++) {
            snprintf(buffer, sizeof(buffer), "element_%d", i);
            bloom_add(bloom, buffer, strlen(buffer));
        }

        // 测试 10000 个未添加的元素
        int false_positives = 0;
        for (int i = 1000; i < 11000; i++) {
            snprintf(buffer, sizeof(buffer), "element_%d", i);
            if (bloom_check(bloom, buffer, strlen(buffer))) {
                false_positives++;
            }
        }

        double actual_fp = (double)false_positives / 10000.0;
        printf("目标假阳性率: %.4f, 实际假阳性率: %.4f (%d/10000)\n",
               target_fp, actual_fp, false_positives);

        bloom_free(bloom);
    }
}

// 演示 3: 统计信息
static void demo_statistics(void) {
    printf("\n=== 演示 3: 统计信息 ===\n");

    bloom_t* bloom = bloom_create(10000, 0.01);
    if (!bloom) return;

    printf("初始状态:\n");
    size_t estimated;
    double fp_rate;
    if (bloom_stats(bloom, &estimated, &fp_rate)) {
        printf("  估算元素数: %zu\n", estimated);
        printf("  当前假阳性率: %.4f%%\n", fp_rate * 100);
    }

    // 添加元素
    char buffer[32];
    printf("\n添加元素过程:\n");
    for (int i = 0; i <= 20000; i += 5000) {
        if (i > 0) {
            for (int j = i - 5000; j < i; j++) {
                snprintf(buffer, sizeof(buffer), "item_%d", j);
                bloom_add(bloom, buffer, strlen(buffer));
            }
        }

        if (bloom_stats(bloom, &estimated, &fp_rate)) {
            printf("  添加 %d 个元素后: 估算=%zu, 假阳性率=%.2f%%\n",
                   i, estimated, fp_rate * 100);
        }
    }

    bloom_free(bloom);
}

// 演示 4: 重置
static void demo_reset(void) {
    printf("\n=== 演示 4: 重置过滤器 ===\n");

    bloom_t* bloom = bloom_create(100, 0.01);
    if (!bloom) return;

    // 添加元素
    bloom_add(bloom, "test1", 5);
    bloom_add(bloom, "test2", 5);
    bloom_add(bloom, "test3", 5);

    printf("添加元素后:\n");
    printf("  \"test1\": %s\n",
           bloom_check(bloom, "test1", 5) ? "存在" : "不存在");
    printf("  \"test2\": %s\n",
           bloom_check(bloom, "test2", 5) ? "存在" : "不存在");

    // 重置
    printf("\n重置过滤器...\n");
    bloom_reset(bloom);

    printf("\n重置后:\n");
    printf("  \"test1\": %s\n",
           bloom_check(bloom, "test1", 5) ? "存在" : "不存在");
    printf("  \"test2\": %s\n",
           bloom_check(bloom, "test2", 5) ? "存在" : "不存在");

    bloom_free(bloom);
}

// 演示 5: 实际应用 - URL 去重
static void demo_url_deduplication(void) {
    printf("\n=== 演示 5: 实际应用 - URL 去重 ===\n");

    bloom_t* bloom = bloom_create(100000, 0.001);
    if (!bloom) return;

    const char* urls[] = {
        "https://example.com/page1",
        "https://example.com/page2",
        "https://example.com/page3",
        "https://example.com/page1",  // 重复
        "https://example.com/page4",
        "https://example.com/page2",  // 重复
    };
    int num_urls = sizeof(urls) / sizeof(urls[0]);

    printf("URL 去重测试:\n\n");
    int new_count = 0;
    int duplicate_count = 0;

    for (int i = 0; i < num_urls; i++) {
        const char* url = urls[i];
        size_t len = strlen(url);

        if (bloom_check(bloom, url, len)) {
            printf("  [重复] %s\n", url);
            duplicate_count++;
        } else {
            printf("  [新URL] %s\n", url);
            bloom_add(bloom, url, len);
            new_count++;
        }
    }

    printf("\n统计:\n");
    printf("  新 URL: %d\n", new_count);
    printf("  重复 URL: %d\n", duplicate_count);

    bloom_free(bloom);
}

// 演示 6: 实际应用 - 密码弱检查
static void demo_password_check(void) {
    printf("\n=== 演示 6: 实际应用 - 弱密码检查 ===\n");

    bloom_t* bloom = bloom_create(10000, 0.001);
    if (!bloom) return;

    // 添加常见弱密码
    const char* weak_passwords[] = {
        "123456", "password", "12345678", "qwerty", "12345",
        "123456789", "letmein", "1234567", "football", "iloveyou"
    };
    int num_weak = sizeof(weak_passwords) / sizeof(weak_passwords[0]);

    printf("加载 %d 个常见弱密码到布隆过滤器\n", num_weak);
    for (int i = 0; i < num_weak; i++) {
        bloom_add(bloom, weak_passwords[i], strlen(weak_passwords[i]));
    }

    // 检查密码
    const char* test_passwords[] = {
        "password",      // 弱密码
        "MyStr0ngP@ss!", // 强密码
        "123456",        // 弱密码
        "SecurePass123", // 强密码
        "qwerty"         // 弱密码
    };
    int num_test = sizeof(test_passwords) / sizeof(test_passwords[0]);

    printf("\n密码强度检查:\n");
    for (int i = 0; i < num_test; i++) {
        const char* pwd = test_passwords[i];
        bool is_weak = bloom_check(bloom, pwd, strlen(pwd));
        printf("  \"%s\": %s\n", pwd, is_weak ? "弱密码 (拒绝)" : "可能安全");
    }

    printf("\n说明: 布隆过滤器可能误判，但不会漏判\n");

    bloom_free(bloom);
}

// 演示 7: 实际应用 - 缓存穿透防护
static void demo_cache_protection(void) {
    printf("\n=== 演示 7: 实际应用 - 缓存穿透防护 ===\n");

    bloom_t* bloom = bloom_create(10000, 0.01);
    if (!bloom) return;

    // 模拟数据库中的用户 ID
    printf("模拟数据库中有用户 ID: 1-1000\n");
    char buffer[32];
    for (int i = 1; i <= 1000; i++) {
        snprintf(buffer, sizeof(buffer), "user_%d", i);
        bloom_add(bloom, buffer, strlen(buffer));
    }

    // 模拟查询
    const char* queries[] = {
        "user_500",    // 存在
        "user_999",    // 存在
        "user_2000",   // 不存在
        "user_5000",   // 不存在
        "user_100"     // 存在
    };
    int num_queries = sizeof(queries) / sizeof(queries[0]);

    printf("\n查询测试:\n");
    int cache_hits = 0;
    int db_queries = 0;
    int prevented = 0;

    for (int i = 0; i < num_queries; i++) {
        const char* query = queries[i];
        bool might_exist = bloom_check(bloom, query, strlen(query));

        if (might_exist) {
            printf("  \"%s\": 可能存在 -> 查询数据库\n", query);
            db_queries++;
            // 模拟数据库查询结果
            if (strncmp(query, "user_", 5) == 0) {
                int id = atoi(query + 5);
                if (id >= 1 && id <= 1000) {
                    printf("    -> 数据库命中\n");
                    cache_hits++;
                } else {
                    printf("    -> 数据库未命中 (假阳性)\n");
                }
            }
        } else {
            printf("  \"%s\": 肯定不存在 -> 直接返回\n", query);
            prevented++;
        }
    }

    printf("\n统计:\n");
    printf("  数据库查询次数: %d\n", db_queries);
    printf("  直接拦截次数: %d\n", prevented);

    bloom_free(bloom);
}

// 演示 8: 不同参数对比
static void demo_parameter_comparison(void) {
    printf("\n=== 演示 8: 不同参数对比 ===\n");

    struct {
        size_t n;
        double p;
    } configs[] = {
        {1000, 0.1},
        {1000, 0.01},
        {1000, 0.001},
        {10000, 0.01},
        {100000, 0.01}
    };
    int num_configs = sizeof(configs) / sizeof(configs[0]);

    printf("配置对比:\n");
    printf("%-12s %-12s %-15s %-15s\n",
           "预期元素", "假阳性率", "估算元素", "实际假阳性率");
    printf("%-12s %-12s %-15s %-15s\n",
           "--------", "--------", "----------", "------------");

    for (int i = 0; i < num_configs; i++) {
        bloom_t* bloom = bloom_create(configs[i].n, configs[i].p);
        if (!bloom) continue;

        // 添加元素
        char buffer[32];
        for (size_t j = 0; j < configs[i].n; j++) {
            snprintf(buffer, sizeof(buffer), "item_%zu", j);
            bloom_add(bloom, buffer, strlen(buffer));
        }

        // 获取统计
        size_t estimated;
        double fp_rate;
        bloom_stats(bloom, &estimated, &fp_rate);

        printf("%-12zu %-12.4f %-15zu %-15.4f%%\n",
               configs[i].n, configs[i].p, estimated, fp_rate * 100);

        bloom_free(bloom);
    }
}

// 演示 9: 大数据量测试
static void demo_large_scale(void) {
    printf("\n=== 演示 9: 大数据量测试 ===\n");

    const size_t n = 100000;
    const double p = 0.001;

    printf("创建布隆过滤器:\n");
    printf("  预期元素: %zu\n", n);
    printf("  假阳性率: %.3f%%\n", p * 100);

    bloom_t* bloom = bloom_create(n, p);
    if (!bloom) return;

    // 添加元素
    printf("\n添加 %zu 个元素...\n", n);
    char buffer[64];
    for (size_t i = 0; i < n; i++) {
        snprintf(buffer, sizeof(buffer), "large_scale_item_%zu", i);
        bloom_add(bloom, buffer, strlen(buffer));
    }
    printf("完成\n");

    // 测试假阳性率
    printf("\n测试假阳性率...\n");
    int false_positives = 0;
    const int test_count = 100000;
    for (int i = 0; i < test_count; i++) {
        snprintf(buffer, sizeof(buffer), "test_item_%d", i);
        if (bloom_check(bloom, buffer, strlen(buffer))) {
            false_positives++;
        }
    }

    double actual_fp = (double)false_positives / test_count;
    printf("  测试次数: %d\n", test_count);
    printf("  假阳性: %d\n", false_positives);
    printf("  实际假阳性率: %.4f%%\n", actual_fp * 100);

    bloom_free(bloom);
}

// 演示 10: 内存使用分析
static void demo_memory_usage(void) {
    printf("\n=== 演示 10: 内存使用分析 ===\n");

    printf("布隆过滤器 vs 哈希表内存对比:\n\n");

    struct {
        size_t n;
        double p;
    } configs[] = {
        {1000, 0.01},
        {10000, 0.01},
        {100000, 0.01},
        {1000000, 0.01}
    };
    int num_configs = sizeof(configs) / sizeof(configs[0]);

    printf("%-12s %-12s %-20s %-20s\n",
           "元素数", "假阳性率", "布隆过滤器(估算)", "哈希表(估算)");
    printf("%-12s %-12s %-20s %-20s\n",
           "------", "--------", "----------------", "------------");

    for (int i = 0; i < num_configs; i++) {
        // 布隆过滤器大小估算: m = -n*ln(p) / (ln(2)^2)
        double m = -configs[i].n * log(configs[i].p) / (log(2) * log(2));
        size_t bloom_bytes = (size_t)(m / 8);

        // 哈希表大小估算: 每个元素约 32 字节（键+值+开销）
        size_t hash_bytes = configs[i].n * 32;

        printf("%-12zu %-12.4f %-20zu %-20zu\n",
               configs[i].n, configs[i].p, bloom_bytes, hash_bytes);
    }

    printf("\n说明: 布隆过滤器在内存使用上远优于哈希表\n");
}

int main(void) {
    printf("========================================\n");
    printf("    布隆过滤器演示程序\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_false_positive_rate();
    demo_statistics();
    demo_reset();
    demo_url_deduplication();
    demo_password_check();
    demo_cache_protection();
    demo_parameter_comparison();
    demo_large_scale();
    demo_memory_usage();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
