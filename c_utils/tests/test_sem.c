#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../c_utils/utest.h"
#include "../c_utils/sem.h"

static const char* test_sem_name = "/test_c_utils_sem";

static void cleanup_sem() {
    sem_handle_t sem = sem_open_ex(test_sem_name, NULL);
    if (sem) {
        sem_close_delete(sem, test_sem_name);
    }
}

void test_sem_create_close() {
    TEST(Sem_CreateClose);
    cleanup_sem();
    
    sem_handle_t sem = sem_create(test_sem_name, 1);
    EXPECT_TRUE(sem != NULL);
    
    sem_close_delete(sem, test_sem_name);
}

void test_sem_create_ex() {
    TEST(Sem_CreateEx);
    cleanup_sem();
    
    sem_handle_t sem = sem_create_ex(test_sem_name, 2, NULL, NULL);
    EXPECT_TRUE(sem != NULL);
    
    sem_close_delete(sem, test_sem_name);
}

void test_sem_p_v() {
    TEST(Sem_PV);
    cleanup_sem();
    
    sem_handle_t sem = sem_create(test_sem_name, 1);
    EXPECT_TRUE(sem != NULL);
    
    sem_p(sem);
    sem_v(sem);
    
    sem_close_delete(sem, test_sem_name);
}

void test_sem_open_ex() {
    TEST(Sem_OpenEx);
    cleanup_sem();
    
    sem_handle_t sem1 = sem_create(test_sem_name, 1);
    EXPECT_TRUE(sem1 != NULL);
    
    sem_handle_t sem2 = sem_open_ex(test_sem_name, NULL);
    EXPECT_TRUE(sem2 != NULL);
    
    sem_close_delete(sem1, test_sem_name);
    if (sem2) {
        sem_close_delete(sem2, NULL);
    }
}

int main() {
    test_sem_create_close();
    test_sem_create_ex();
    test_sem_p_v();
    test_sem_open_ex();

    return 0;
}
