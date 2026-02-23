#ifndef C_UTILS_THREADPOOL_H
#define C_UTILS_THREADPOOL_H

#include <stddef.h>
#include <stdbool.h>

typedef struct threadpool_s threadpool_t;
typedef struct threadpool_task_s threadpool_task_t;

// 任务优先级
typedef enum {
    THREADPOOL_PRIORITY_LOW = 0,
    THREADPOOL_PRIORITY_NORMAL = 1,
    THREADPOOL_PRIORITY_HIGH = 2
} threadpool_priority_t;

// 创建与销毁
// num_threads: 线程池中的工作线程数量 (0 表示使用 CPU 核心数)
threadpool_t* threadpool_create(int num_threads);
void          threadpool_destroy(threadpool_t *pool);

// 基本任务提交
// func: 任务函数, arg: 传给函数的参数
// 返回任务 ID (可用于取消), 失败返回 0
int threadpool_add_task(threadpool_t *pool, void (*func)(void*), void *arg);

// 带优先级的任务提交
int threadpool_add_task_with_priority(threadpool_t *pool, void (*func)(void*), 
                                       void *arg, threadpool_priority_t priority);

// 带结果回调的任务提交
typedef void (*threadpool_result_cb)(void *arg, void *result);
int threadpool_add_task_with_callback(threadpool_t *pool, void *(*func)(void*), 
                                       void *arg, threadpool_result_cb callback);

// 取消任务
// 返回 true 表示成功取消, false 表示任务已在执行或已完成
bool threadpool_cancel_task(threadpool_t *pool, int task_id);

// 等待所有任务完成
// timeout_ms: 超时时间(毫秒), -1 表示无限等待
// 返回 true 表示所有任务完成, false 表示超时
bool threadpool_wait_all(threadpool_t *pool, int timeout_ms);

// 等待特定任务完成
bool threadpool_wait_task(threadpool_t *pool, int task_id, int timeout_ms);

// 暂停/恢复线程池
void threadpool_pause(threadpool_t *pool);
void threadpool_resume(threadpool_t *pool);

// 动态调整线程数
// 返回实际调整的线程数
int threadpool_resize(threadpool_t *pool, int new_num_threads);

// 属性获取
int  threadpool_get_thread_count(const threadpool_t *pool);
int  threadpool_get_active_count(const threadpool_t *pool);
int  threadpool_get_pending_count(const threadpool_t *pool);
int  threadpool_get_completed_count(const threadpool_t *pool);
bool threadpool_is_paused(const threadpool_t *pool);
bool threadpool_is_shutdown(const threadpool_t *pool);

// 清理已完成任务的状态 (释放相关内存)
void threadpool_cleanup_completed(threadpool_t *pool);

#endif // C_UTILS_THREADPOOL_H
