#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../c_utils/utest.h"
#include "../c_utils/threadpool.h"

static int counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static void increment_counter(void *arg) {
    (void)arg;
    pthread_mutex_lock(&counter_mutex);
    counter++;
    pthread_mutex_unlock(&counter_mutex);
    usleep(10000);
}

static void *increment_and_return(void *arg) {
    (void)arg;
    pthread_mutex_lock(&counter_mutex);
    counter++;
    pthread_mutex_unlock(&counter_mutex);
    usleep(10000);
    return (void*)(long)counter;
}

static int callback_count = 0;
static void *callback_result = NULL;

static void test_callback(void *arg, void *result) {
    (void)arg;
    pthread_mutex_lock(&counter_mutex);
    callback_count++;
    callback_result = result;
    pthread_mutex_unlock(&counter_mutex);
}

static void reset_test_state(void) {
    pthread_mutex_lock(&counter_mutex);
    counter = 0;
    callback_count = 0;
    callback_result = NULL;
    pthread_mutex_unlock(&counter_mutex);
}

void test_threadpool_create() {
    TEST(Threadpool_Create);
    reset_test_state();
    threadpool_t* pool = threadpool_create(4);
    EXPECT_TRUE(pool != NULL);
    EXPECT_EQ(threadpool_get_thread_count(pool), 4);
    threadpool_destroy(pool);
}

void test_threadpool_create_default() {
    TEST(Threadpool_CreateDefault);
    threadpool_t* pool = threadpool_create(0);
    EXPECT_TRUE(pool != NULL);
    EXPECT_TRUE(threadpool_get_thread_count(pool) > 0);
    threadpool_destroy(pool);
}

void test_threadpool_add_task() {
    TEST(Threadpool_AddTask);
    threadpool_t* pool = threadpool_create(2);
    
    counter = 0;
    for (int i = 0; i < 10; i++) {
        int task_id = threadpool_add_task(pool, increment_counter, NULL);
        EXPECT_TRUE(task_id > 0);
    }
    
    bool completed = threadpool_wait_all(pool, 2000);
    EXPECT_TRUE(completed);
    EXPECT_EQ(counter, 10);
    
    threadpool_destroy(pool);
}

void test_threadpool_wait_all() {
    TEST(Threadpool_WaitAll);
    threadpool_t* pool = threadpool_create(4);
    
    counter = 0;
    for (int i = 0; i < 20; i++) {
        threadpool_add_task(pool, increment_counter, NULL);
    }
    
    bool completed = threadpool_wait_all(pool, 3000);
    EXPECT_TRUE(completed);
    EXPECT_EQ(counter, 20);
    
    threadpool_destroy(pool);
}

void test_threadpool_add_task_with_priority() {
    TEST(Threadpool_AddTaskWithPriority);
    reset_test_state();
    threadpool_t* pool = threadpool_create(1);
    
    threadpool_pause(pool);
    
    int id1 = threadpool_add_task_with_priority(pool, increment_counter, NULL, THREADPOOL_PRIORITY_LOW);
    int id2 = threadpool_add_task_with_priority(pool, increment_counter, NULL, THREADPOOL_PRIORITY_HIGH);
    int id3 = threadpool_add_task_with_priority(pool, increment_counter, NULL, THREADPOOL_PRIORITY_NORMAL);
    
    EXPECT_TRUE(id1 > 0);
    EXPECT_TRUE(id2 > 0);
    EXPECT_TRUE(id3 > 0);
    
    threadpool_resume(pool);
    threadpool_wait_all(pool, 2000);
    
    EXPECT_EQ(counter, 3);
    
    threadpool_destroy(pool);
}

void test_threadpool_callback() {
    TEST(Threadpool_Callback);
    reset_test_state();
    
    threadpool_t* pool = threadpool_create(4);
    EXPECT_TRUE(pool != NULL);
    
    int task_id = threadpool_add_task_with_callback(pool, increment_and_return, NULL, test_callback);
    EXPECT_TRUE(task_id > 0);
    
    threadpool_wait_all(pool, 2000);
    
    EXPECT_EQ(counter, 1);
    EXPECT_EQ(callback_count, 1);
    
    threadpool_destroy(pool);
}

void test_threadpool_cancel_task() {
    TEST(Threadpool_CancelTask);
    reset_test_state();
    
    threadpool_t* pool = threadpool_create(1);
    EXPECT_TRUE(pool != NULL);
    
    threadpool_pause(pool);
    
    int task_id1 = threadpool_add_task(pool, increment_counter, NULL);
    int task_id2 = threadpool_add_task(pool, increment_counter, NULL);
    
    EXPECT_TRUE(task_id1 > 0);
    EXPECT_TRUE(task_id2 > 0);
    
    bool cancelled = threadpool_cancel_task(pool, task_id2);
    EXPECT_TRUE(cancelled);
    
    threadpool_resume(pool);
    
    threadpool_wait_all(pool, 2000);
    
    EXPECT_EQ(counter, 1);
    
    threadpool_destroy(pool);
}

void test_threadpool_wait_task() {
    TEST(Threadpool_WaitTask);
    reset_test_state();
    
    threadpool_t* pool = threadpool_create(4);
    EXPECT_TRUE(pool != NULL);
    
    int task_id = threadpool_add_task(pool, increment_counter, NULL);
    EXPECT_TRUE(task_id > 0);
    
    bool result = threadpool_wait_task(pool, task_id, 2000);
    EXPECT_TRUE(result);
    EXPECT_EQ(counter, 1);
    
    threadpool_destroy(pool);
}

void test_threadpool_resize() {
    TEST(Threadpool_Resize);
    reset_test_state();
    
    threadpool_t* pool = threadpool_create(4);
    EXPECT_TRUE(pool != NULL);
    EXPECT_EQ(threadpool_get_thread_count(pool), 4);
    
    int new_count = threadpool_resize(pool, 2);
    EXPECT_EQ(new_count, 2);
    
    threadpool_destroy(pool);
}

void test_threadpool_null_params() {
    TEST(Threadpool_NullParams);
    
    EXPECT_TRUE(threadpool_create(0) != NULL);
    threadpool_destroy(NULL);
    
    EXPECT_EQ(threadpool_add_task(NULL, increment_counter, NULL), 0);
    EXPECT_EQ(threadpool_add_task_with_priority(NULL, increment_counter, NULL, THREADPOOL_PRIORITY_NORMAL), 0);
    EXPECT_EQ(threadpool_add_task_with_callback(NULL, increment_and_return, NULL, test_callback), 0);
    
    EXPECT_TRUE(threadpool_cancel_task(NULL, 1) == false);
    EXPECT_TRUE(threadpool_wait_all(NULL, 1000) == false);
    EXPECT_TRUE(threadpool_wait_task(NULL, 1, 1000) == false);
    
    threadpool_pause(NULL);
    threadpool_resume(NULL);
    
    EXPECT_EQ(threadpool_resize(NULL, 4), 0);
    
    EXPECT_EQ(threadpool_get_thread_count(NULL), 0);
    EXPECT_EQ(threadpool_get_active_count(NULL), 0);
    EXPECT_EQ(threadpool_get_pending_count(NULL), 0);
    EXPECT_EQ(threadpool_get_completed_count(NULL), 0);
    EXPECT_TRUE(threadpool_is_paused(NULL) == false);
    EXPECT_TRUE(threadpool_is_shutdown(NULL) == true);
    
    threadpool_cleanup_completed(NULL);
}

void test_threadpool_pause_resume() {
    TEST(Threadpool_PauseResume);
    reset_test_state();
    threadpool_t* pool = threadpool_create(2);
    
    threadpool_pause(pool);
    EXPECT_TRUE(threadpool_is_paused(pool));
    
    counter = 0;
    for (int i = 0; i < 5; i++) {
        threadpool_add_task(pool, increment_counter, NULL);
    }
    
    usleep(100000);
    EXPECT_EQ(threadpool_get_pending_count(pool), 5);
    
    threadpool_resume(pool);
    EXPECT_FALSE(threadpool_is_paused(pool));
    
    threadpool_wait_all(pool, 2000);
    EXPECT_EQ(counter, 5);
    
    threadpool_destroy(pool);
}

void test_threadpool_get_counts() {
    TEST(Threadpool_GetCounts);
    threadpool_t* pool = threadpool_create(2);
    
    EXPECT_EQ(threadpool_get_thread_count(pool), 2);
    EXPECT_EQ(threadpool_get_active_count(pool), 0);
    EXPECT_EQ(threadpool_get_pending_count(pool), 0);
    EXPECT_EQ(threadpool_get_completed_count(pool), 0);
    
    threadpool_destroy(pool);
}

void test_threadpool_destroy_null() {
    TEST(Threadpool_DestroyNull);
    threadpool_destroy(NULL);
}

void test_threadpool_add_task_null_func() {
    TEST(Threadpool_AddTaskNullFunc);
    threadpool_t* pool = threadpool_create(2);
    
    int task_id = threadpool_add_task(pool, NULL, NULL);
    EXPECT_TRUE(task_id == 0);
    
    threadpool_destroy(pool);
}

void test_threadpool_is_shutdown() {
    TEST(Threadpool_IsShutdown);
    reset_test_state();
    threadpool_t* pool = threadpool_create(2);
    
    EXPECT_FALSE(threadpool_is_shutdown(pool));
    
    threadpool_destroy(pool);
}

void test_threadpool_cleanup_completed() {
    TEST(Threadpool_CleanupCompleted);
    threadpool_t* pool = threadpool_create(2);
    
    for (int i = 0; i < 5; i++) {
        threadpool_add_task(pool, increment_counter, NULL);
    }
    
    threadpool_wait_all(pool, 2000);
    threadpool_cleanup_completed(pool);
    
    threadpool_destroy(pool);
}

void test_threadpool_stress_many_tasks() {
    TEST(Threadpool_StressManyTasks);
    threadpool_t* pool = threadpool_create(4);
    
    counter = 0;
    for (int i = 0; i < 100; i++) {
        threadpool_add_task(pool, increment_counter, NULL);
    }
    
    bool completed = threadpool_wait_all(pool, 5000);
    EXPECT_TRUE(completed);
    EXPECT_EQ(counter, 100);
    
    threadpool_destroy(pool);
}

void test_threadpool_edge_case_single_thread() {
    TEST(Threadpool_EdgeCaseSingleThread);
    threadpool_t* pool = threadpool_create(1);
    
    counter = 0;
    for (int i = 0; i < 10; i++) {
        threadpool_add_task(pool, increment_counter, NULL);
    }
    
    threadpool_wait_all(pool, 3000);
    EXPECT_EQ(counter, 10);
    
    threadpool_destroy(pool);
}

void test_threadpool_edge_case_many_threads() {
    TEST(Threadpool_EdgeCaseManyThreads);
    threadpool_t* pool = threadpool_create(16);
    
    counter = 0;
    for (int i = 0; i < 100; i++) {
        threadpool_add_task(pool, increment_counter, NULL);
    }
    
    threadpool_wait_all(pool, 3000);
    EXPECT_EQ(counter, 100);
    
    threadpool_destroy(pool);
}

int main() {
    test_threadpool_create();
    test_threadpool_create_default();
    test_threadpool_add_task();
    test_threadpool_wait_all();
    test_threadpool_add_task_with_priority();
    test_threadpool_callback();
    test_threadpool_cancel_task();
    test_threadpool_wait_task();
    test_threadpool_resize();
    test_threadpool_null_params();
    test_threadpool_pause_resume();
    test_threadpool_get_counts();
    test_threadpool_destroy_null();
    test_threadpool_add_task_null_func();
    test_threadpool_is_shutdown();
    test_threadpool_cleanup_completed();
    test_threadpool_stress_many_tasks();
    test_threadpool_edge_case_single_thread();
    test_threadpool_edge_case_many_threads();

    return 0;
}
