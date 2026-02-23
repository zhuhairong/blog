#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/utest.h"
#include "../c_utils/threadpool.h"

static int counter = 0;

static void increment_counter(void *arg) {
    (void)arg;
    __sync_fetch_and_add(&counter, 1);
}

void test_threadpool_create() {
    TEST(Threadpool_Create);
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
    threadpool_t* pool = threadpool_create(2);
    
    counter = 0;
    int low_id = threadpool_add_task_with_priority(pool, increment_counter, NULL, THREADPOOL_PRIORITY_LOW);
    int normal_id = threadpool_add_task_with_priority(pool, increment_counter, NULL, THREADPOOL_PRIORITY_NORMAL);
    int high_id = threadpool_add_task_with_priority(pool, increment_counter, NULL, THREADPOOL_PRIORITY_HIGH);
    
    EXPECT_TRUE(low_id > 0);
    EXPECT_TRUE(normal_id > 0);
    EXPECT_TRUE(high_id > 0);
    
    threadpool_wait_all(pool, 2000);
    EXPECT_EQ(counter, 3);
    
    threadpool_destroy(pool);
}

void test_threadpool_pause_resume() {
    TEST(Threadpool_PauseResume);
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
