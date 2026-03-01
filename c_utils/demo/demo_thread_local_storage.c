#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/thread_local_storage.h"

// 线程数据结构体
typedef struct {
    int thread_id;
    char thread_name[32];
    int counter;
} thread_data_t;

// 析构函数
void destructor(void *value) {
    if (value) {
        thread_data_t *data = (thread_data_t *)value;
        printf("   析构线程数据: ID=%d, Name=%s\n", data->thread_id, data->thread_name);
        free(data);
    }
}

// 线程函数
void* thread_func(void *arg) {
    int thread_num = *((int *)arg);
    tls_key_t key;
    tls_config_t config;
    tls_state_t state;
    tls_config_init(&config);
    tls_state_init(&state);
    
    // 创建TLS键
    tls_key_create_ex(&key, destructor, &config, &state);
    
    // 创建线程数据
    thread_data_t *data = (thread_data_t *)malloc(sizeof(thread_data_t));
    data->thread_id = thread_num;
    sprintf(data->thread_name, "Thread-%d", thread_num);
    data->counter = 0;
    
    // 设置TLS值
    tls_set_value_ex(key, data, &config, &state);
    
    // 模拟一些操作
    for (int i = 0; i < 5; i++) {
        // 获取TLS值
        thread_data_t *current_data = (thread_data_t *)tls_get_value(key);
        if (current_data) {
            current_data->counter++;
            printf("   %s: Counter = %d\n", current_data->thread_name, current_data->counter);
        }
        // 休眠一段时间
        usleep(100000);
    }
    
    // 获取线程ID
    uint64_t thread_id;
    tls_get_thread_id(&thread_id, &state);
    printf("   %s: Thread ID = %llu\n", data->thread_name, (unsigned long long)thread_id);
    
    // 验证键
    bool valid;
    tls_validate_key(key, &valid, &state);
    printf("   %s: Key valid = %s\n", data->thread_name, valid ? "true" : "false");
    
    // 清理线程TLS
    tls_cleanup_thread(&state);
    
    // 删除键
    tls_key_delete(key, &state);
    
    return NULL;
}

int main() {
    printf("=== Thread Local Storage Demo ===\n\n");
    
    // 测试基本TLS操作
    printf("1. 基本TLS操作测试:\n");
    tls_key_t key;
    tls_config_t config;
    tls_state_t state;
    tls_config_init(&config);
    tls_state_init(&state);
    
    // 创建TLS键
    tls_key_create_ex(&key, destructor, &config, &state);
    printf("   TLS键创建成功\n");
    
    // 设置TLS值
    int test_value = 42;
    tls_set_value_ex(key, &test_value, &config, &state);
    printf("   设置TLS值: %d\n", test_value);
    
    // 获取TLS值
    void *value;
    tls_get_value_ex(key, &value, &config, &state);
    if (value) {
        printf("   获取TLS值: %d\n", *((int *)value));
    }
    
    // 验证键
    bool valid;
    tls_validate_key(key, &valid, &state);
    printf("   键验证: %s\n", valid ? "通过" : "失败");
    
    // 获取线程ID
    uint64_t thread_id;
    tls_get_thread_id(&thread_id, &state);
    printf("   当前线程ID: %llu\n", (unsigned long long)thread_id);
    
    // 删除键
    tls_key_delete(key, &state);
    printf("   TLS键删除成功\n");
    
    // 测试多线程
    printf("\n2. 多线程TLS测试:\n");
    pthread_t threads[3];
    int thread_args[3] = {1, 2, 3};
    
    // 创建线程
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, &thread_args[i]);
    }
    
    // 等待线程完成
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 测试批量操作
    printf("\n3. 批量操作测试:\n");
    tls_key_t batch_keys[2];
    int batch_values[2] = {100, 200};
    void *batch_values_ptr[2] = {&batch_values[0], &batch_values[1]};
    
    // 批量创建键
    tls_batch_key_create(batch_keys, 2, NULL, &config, &state);
    printf("   批量创建键成功\n");
    
    // 批量设置值
    tls_batch_set_value(batch_keys, batch_values_ptr, 2, &config, &state);
    printf("   批量设置值成功\n");
    
    // 批量获取值
    void *batch_results[2];
    tls_batch_get_value(batch_keys, batch_results, 2, &config, &state);
    printf("   批量获取值: %d, %d\n", *((int *)batch_results[0]), *((int *)batch_results[1]));
    
    // 批量删除键
    tls_batch_key_delete(batch_keys, 2, &state);
    printf("   批量删除键成功\n");
    
    // 测试统计信息
    printf("\n4. 统计信息:\n");
    tls_get_statistics(&state);
    printf("   键创建次数: %zu\n", state.key_creations);
    printf("   键删除次数: %zu\n", state.key_deletions);
    printf("   值设置次数: %zu\n", state.value_sets);
    printf("   值获取次数: %zu\n", state.value_gets);
    printf("   活跃键数量: %zu\n", state.active_keys);
    printf("   活跃线程数量: %zu\n", state.active_threads);
    
    // 重置状态
    tls_reset_state(&state);
    printf("   状态已重置\n");
    
    printf("\n=== Thread Local Storage Demo 完成 ===\n");
    return 0;
}