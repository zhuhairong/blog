#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/trie.h"

// 遍历回调函数
bool traverse_callback(const char *key, void *value, void *user_data) {
    printf("   %s -> %s\n", key, (char *)value);
    return true; // 继续遍历
}

int main() {
    printf("=== Trie Demo ===\n\n");
    
    // 创建trie
    printf("1. 创建Trie:\n");
    trie_t *trie = trie_create();
    if (trie) {
        printf("   Trie创建成功\n");
    } else {
        printf("   Trie创建失败\n");
        return 1;
    }
    
    // 插入键值对
    printf("\n2. 插入键值对:\n");
    trie_error_t error;
    
    error = trie_insert(trie, "apple", "水果");
    printf("   插入 'apple' -> '水果': %s\n", error == TRIE_OK ? "成功" : trie_strerror(error));
    
    error = trie_insert(trie, "app", "应用");
    printf("   插入 'app' -> '应用': %s\n", error == TRIE_OK ? "成功" : trie_strerror(error));
    
    error = trie_insert(trie, "banana", "香蕉");
    printf("   插入 'banana' -> '香蕉': %s\n", error == TRIE_OK ? "成功" : trie_strerror(error));
    
    error = trie_insert(trie, "orange", "橙子");
    printf("   插入 'orange' -> '橙子': %s\n", error == TRIE_OK ? "成功" : trie_strerror(error));
    
    error = trie_insert(trie, "grape", "葡萄");
    printf("   插入 'grape' -> '葡萄': %s\n", error == TRIE_OK ? "成功" : trie_strerror(error));
    
    // 检查键是否存在
    printf("\n3. 检查键是否存在:\n");
    printf("   'apple' 存在: %s\n", trie_contains(trie, "apple") ? "是" : "否");
    printf("   'app' 存在: %s\n", trie_contains(trie, "app") ? "是" : "否");
    printf("   'banana' 存在: %s\n", trie_contains(trie, "banana") ? "是" : "否");
    printf("   'pear' 存在: %s\n", trie_contains(trie, "pear") ? "是" : "否");
    
    // 获取值
    printf("\n4. 获取值:\n");
    char *value;
    
    value = (char *)trie_get(trie, "apple");
    printf("   'apple' 的值: %s\n", value ? value : "不存在");
    
    value = (char *)trie_get(trie, "app");
    printf("   'app' 的值: %s\n", value ? value : "不存在");
    
    value = (char *)trie_get(trie, "pear");
    printf("   'pear' 的值: %s\n", value ? value : "不存在");
    
    // 前缀搜索
    printf("\n5. 前缀搜索:\n");
    char *keys[10];
    void *values[10];
    size_t count = trie_prefix_search(trie, "app", keys, values, 10);
    printf("   前缀 'app' 的匹配结果 (%zu 个):\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("   %s -> %s\n", keys[i], (char *)values[i]);
        free(keys[i]); // 释放分配的内存
    }
    
    // 遍历trie
    printf("\n6. 遍历Trie:\n");
    trie_traverse(trie, traverse_callback, NULL);
    
    // 获取trie信息
    printf("\n7. Trie信息:\n");
    printf("   大小: %zu\n", trie_size(trie));
    printf("   内存使用: %zu 字节\n", trie_memory_usage(trie));
    
    // 删除键
    printf("\n8. 删除键:\n");
    printf("   删除 'banana': %s\n", trie_remove(trie, "banana") ? "成功" : "失败");
    printf("   'banana' 存在: %s\n", trie_contains(trie, "banana") ? "是" : "否");
    
    // 再次遍历trie
    printf("\n9. 再次遍历Trie:\n");
    trie_traverse(trie, traverse_callback, NULL);
    
    // 清空trie
    printf("\n10. 清空Trie:\n");
    trie_clear(trie);
    printf("   清空后大小: %zu\n", trie_size(trie));
    
    // 测试错误处理
    printf("\n11. 错误处理测试:\n");
    error = trie_insert(trie, "", "空键");
    printf("   插入空键: %s\n", trie_strerror(error));
    
    // 释放trie
    trie_free(trie);
    printf("   Trie已释放\n");
    
    // 测试带配置的trie
    printf("\n12. 带配置的Trie测试:\n");
    trie_config_t config;
    config.case_sensitive = false; // 不区分大小写
    config.allow_duplicates = false;
    config.max_depth = 10;
    config.max_children = 0; // 无限制
    config.value_free = NULL;
    
    trie_t *trie_with_config = trie_create_with_config(&config);
    if (trie_with_config) {
        printf("   带配置的Trie创建成功\n");
        
        error = trie_insert(trie_with_config, "Apple", "苹果");
        printf("   插入 'Apple' -> '苹果': %s\n", error == TRIE_OK ? "成功" : trie_strerror(error));
        
        // 测试不区分大小写
        value = (char *)trie_get(trie_with_config, "apple");
        printf("   获取 'apple' 的值: %s (不区分大小写)\n", value ? value : "不存在");
        
        trie_free(trie_with_config);
        printf("   带配置的Trie已释放\n");
    } else {
        printf("   带配置的Trie创建失败\n");
    }
    
    printf("\n=== Trie Demo 完成 ===\n");
    return 0;
}