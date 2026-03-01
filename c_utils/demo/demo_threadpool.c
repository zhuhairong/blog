/**
 * 线程池演示程序
 *
 * 功能：
 * - 基本任务提交
 * - 带优先级的任务
 * - 带回调的任务
 * - 任务取消
 * - 线程池管理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/threadpool.h"

// 全局变量，用于演示任务计数
int g_task_count = 0;
int g_completed_count = 0;

// 基本任务函数
static void task_function(void *arg) {
    int task_id = *((int *)arg);
    printf("任务 %d 开始执行\n", task_id);
    
    // 模拟任务执行时间
    usleep(500000); // 500ms
    
    printf("任务 %d 执行完成\n", task_id);
    
    g_completed_count++;
    free(arg);
}

// 带返回值的任务函数
static void *task_with_result(void *arg) {
    int task_id = *((int *)arg);
    printf("带返回值任务 %d 开始执行\n", task_id);
    
    // 模拟任务执行时间
    usleep(300000); // 300ms
    
    printf("带返回值任务 %d 执行完成\n", task_id);
    
    int *result = malloc(sizeof(int));
    *result = task_id * 10;
    
    free(arg);
    return result;
}

// 任务结果回调函数
static void result_callback(void *arg, void *result) {
    int task_id = *((int *)arg);
    int *result_value = (int *)result;
    printf("任务 %d 的回调被调用，结果: %d\n", task_id, *result_value);
    free(result);
    free(arg);
}

// 演示 1: 基本线程池创建和任务提交
static void demo_basic_threadpool(void) {
    printf("\n=== 演示 1: 基本线程池创建和任务提交 ===\n");

    // 创建线程池，使用4个线程
    threadpool_t *pool = threadpool_create(4);
    if (!pool) {
        printf("创建线程池失败\n");
        return;
    }

    printf("线程池创建成功，线程数: %d\n", threadpool_get_thread_count(pool));

    // 提交10个任务
    g_task_count = 10;
    g_completed_count = 0;

    for (int i = 1; i <= g_task_count; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        int task_handle = threadpool_add_task(pool, task_function, task_id);
        if (task_handle > 0) {
            printf("提交任务 %d，任务ID: %d\n", i, task_handle);
        } else {
            printf("提交任务 %d 失败\n", i);
            free(task_id);
        }
    }

    // 等待所有任务完成
    printf("等待所有任务完成...\n");
    threadpool_wait_all(pool, -1);

    printf("所有任务执行完成，共完成 %d 个任务\n", g_completed_count);

    // 销毁线程池
    threadpool_destroy(pool);
}

// 演示 2: 带优先级的任务
static void demo_priority_tasks(void) {
    printf("\n=== 演示 2: 带优先级的任务 ===\n");

    // 创建线程池，使用2个线程
    threadpool_t *pool = threadpool_create(2);
    if (!pool) {
        printf("创建线程池失败\n");
        return;
    }

    // 提交不同优先级的任务
    for (int i = 1; i <= 5; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        
        // 任务1和2设置为高优先级
        threadpool_priority_t priority = (i <= 2) ? THREADPOOL_PRIORITY_HIGH : THREADPOOL_PRIORITY_NORMAL;
        int task_handle = threadpool_add_task_with_priority(pool, task_function, task_id, priority);
        
        printf("提交任务 %d，优先级: %s，任务ID: %d\n", 
               i, 
               (priority == THREADPOOL_PRIORITY_HIGH) ? "高" : "正常", 
               task_handle);
    }

    // 等待所有任务完成
    printf("等待所有任务完成...\n");
    threadpool_wait_all(pool, -1);

    // 销毁线程池
    threadpool_destroy(pool);
}

// 演示 3: 带回调的任务
static void demo_tasks_with_callback(void) {
    printf("\n=== 演示 3: 带回调的任务 ===\n");

    // 创建线程池，使用3个线程
    threadpool_t *pool = threadpool_create(3);
    if (!pool) {
        printf("创建线程池失败\n");
        return;
    }

    // 提交带回调的任务
    for (int i = 1; i <= 3; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        
        int task_handle = threadpool_add_task_with_callback(pool, task_with_result, task_id, result_callback);
        if (task_handle > 0) {
            printf("提交带回调任务 %d，任务ID: %d\n", i, task_handle);
        } else {
            printf("提交任务 %d 失败\n", i);
            free(task_id);
        }
    }

    // 等待所有任务完成
    printf("等待所有任务完成...\n");
    threadpool_wait_all(pool, -1);

    // 销毁线程池
    threadpool_destroy(pool);
}

// 演示 4: 任务取消
static void demo_task_cancellation(void) {
    printf("\n=== 演示 4: 任务取消 ===\n");

    // 创建线程池，使用2个线程
    threadpool_t *pool = threadpool_create(2);
    if (!pool) {
        printf("创建线程池失败\n");
        return;
    }

    // 提交5个任务
    int task_handles[5];
    for (int i = 1; i <= 5; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        task_handles[i-1] = threadpool_add_task(pool, task_function, task_id);
        printf("提交任务 %d，任务ID: %d\n", i, task_handles[i-1]);
    }

    // 等待一段时间，然后取消任务3和5
    usleep(100000); // 100ms
    
    bool cancelled3 = threadpool_cancel_task(pool, task_handles[2]); // 任务3
    bool cancelled5 = threadpool_cancel_task(pool, task_handles[4]); // 任务5
    
    printf("取消任务3: %s\n", cancelled3 ? "成功" : "失败");
    printf("取消任务5: %s\n", cancelled5 ? "成功" : "失败");

    // 等待所有任务完成
    printf("等待所有任务完成...\n");
    threadpool_wait_all(pool, -1);

    // 销毁线程池
    threadpool_destroy(pool);
}

// 演示 5: 线程池管理
static void demo_threadpool_management(void) {
    printf("\n=== 演示 5: 线程池管理 ===\n");

    // 创建线程池，使用2个线程
    threadpool_t *pool = threadpool_create(2);
    if (!pool) {
        printf("创建线程池失败\n");
        return;
    }

    printf("初始线程数: %d\n", threadpool_get_thread_count(pool));

    // 提交一些任务
    for (int i = 1; i <= 4; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        threadpool_add_task(pool, task_function, task_id);
    }

    // 动态调整线程数
    int new_thread_count = 4;
    int actual = threadpool_resize(pool, new_thread_count);
    printf("调整线程数到 %d，实际调整到: %d\n", new_thread_count, actual);

    // 暂停线程池
    threadpool_pause(pool);
    printf("线程池已暂停\n");

    // 提交任务（这些任务会在恢复后执行）
    int *task_id = malloc(sizeof(int));
    *task_id = 5;
    threadpool_add_task(pool, task_function, task_id);
    printf("在暂停状态下提交任务5\n");

    // 恢复线程池
    threadpool_resume(pool);
    printf("线程池已恢复\n");

    // 等待所有任务完成
    printf("等待所有任务完成...\n");
    threadpool_wait_all(pool, -1);

    // 清理已完成的任务
    threadpool_cleanup_completed(pool);
    printf("已清理完成的任务\n");

    // 销毁线程池
    threadpool_destroy(pool);
}

// 演示 6: 线程池状态查询
static void demo_threadpool_status(void) {
    printf("\n=== 演示 6: 线程池状态查询 ===\n");

    // 创建线程池，使用3个线程
    threadpool_t *pool = threadpool_create(3);
    if (!pool) {
        printf("创建线程池失败\n");
        return;
    }

    // 初始状态
    printf("初始状态:\n");
    printf("  线程数: %d\n", threadpool_get_thread_count(pool));
    printf("  活跃线程数: %d\n", threadpool_get_active_count(pool));
    printf("  待处理任务数: %d\n", threadpool_get_pending_count(pool));
    printf("  已完成任务数: %d\n", threadpool_get_completed_count(pool));
    printf("  是否暂停: %s\n", threadpool_is_paused(pool) ? "是" : "否");
    printf("  是否关闭: %s\n", threadpool_is_shutdown(pool) ? "是" : "否");

    // 提交一些任务
    for (int i = 1; i <= 5; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        threadpool_add_task(pool, task_function, task_id);
    }

    // 等待一段时间后查询状态
    usleep(200000); // 200ms
    printf("\n任务执行中状态:\n");
    printf("  活跃线程数: %d\n", threadpool_get_active_count(pool));
    printf("  待处理任务数: %d\n", threadpool_get_pending_count(pool));
    printf("  已完成任务数: %d\n", threadpool_get_completed_count(pool));

    // 等待所有任务完成
    threadpool_wait_all(pool, -1);

    // 最终状态
    printf("\n最终状态:\n");
    printf("  活跃线程数: %d\n", threadpool_get_active_count(pool));
    printf("  待处理任务数: %d\n", threadpool_get_pending_count(pool));
    printf("  已完成任务数: %d\n", threadpool_get_completed_count(pool));

    // 销毁线程池
    threadpool_destroy(pool);
}

// 演示 7: 线程池大小自动调整
static void demo_threadpool_auto_size(void) {
    printf("\n=== 演示 7: 线程池大小自动调整 ===\n");

    // 创建线程池，使用0表示自动使用CPU核心数
    threadpool_t *pool = threadpool_create(0);
    if (!pool) {
        printf("创建线程池失败\n");
        return;
    }

    int thread_count = threadpool_get_thread_count(pool);
    printf("自动线程数: %d\n", thread_count);

    // 提交一些任务
    for (int i = 1; i <= thread_count * 2; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i;
        threadpool_add_task(pool, task_function, task_id);
    }

    // 等待所有任务完成
    printf("等待所有任务完成...\n");
    threadpool_wait_all(pool, -1);

    // 销毁线程池
    threadpool_destroy(pool);
}

// 演示 8: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 8: 错误处理 ===\n");

    // 测试创建线程池失败的情况
    threadpool_t *pool = threadpool_create(-1); // 无效的线程数
    if (!pool) {
        printf("测试创建线程池失败: 无效的线程数\n");
    }

    // 创建有效的线程池
    pool = threadpool_create(2);
    if (pool) {
        // 测试取消不存在的任务
        bool cancelled = threadpool_cancel_task(pool, 9999);
        printf("取消不存在的任务: %s\n", cancelled ? "成功" : "失败");

        // 销毁线程池
        threadpool_destroy(pool);
    }
}

int main(void) {
    printf("========================================\n");
    printf("    线程池演示\n");
    printf("========================================\n");

    demo_basic_threadpool();
    demo_priority_tasks();
    demo_tasks_with_callback();
    demo_task_cancellation();
    demo_threadpool_management();
    demo_threadpool_status();
    demo_threadpool_auto_size();
    demo_error_handling();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
