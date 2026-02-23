#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/skiplist.h"

static int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void test_skiplist_create() {
    TEST(Skiplist_Create);
    skiplist_t* sl = skiplist_create(int_compare);
    EXPECT_TRUE(sl != NULL);
    skiplist_free(sl);
}

void test_skiplist_insert_get() {
    TEST(Skiplist_InsertGet);
    skiplist_t* sl = skiplist_create(int_compare);
    
    int key = 10;
    int value = 100;
    skiplist_insert(sl, &key, &value);
    
    int* result = (int*)skiplist_get(sl, &key);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*result, 100);
    
    skiplist_free(sl);
}

void test_skiplist_get_not_found() {
    TEST(Skiplist_GetNotFound);
    skiplist_t* sl = skiplist_create(int_compare);
    
    int key1 = 10;
    int value = 100;
    skiplist_insert(sl, &key1, &value);
    
    int key2 = 20;
    void* result = skiplist_get(sl, &key2);
    EXPECT_TRUE(result == NULL);
    
    skiplist_free(sl);
}

void test_skiplist_delete() {
    TEST(Skiplist_Delete);
    skiplist_t* sl = skiplist_create(int_compare);
    
    int key = 10;
    int value = 100;
    skiplist_insert(sl, &key, &value);
    
    int* result = (int*)skiplist_get(sl, &key);
    EXPECT_TRUE(result != NULL);
    
    skiplist_delete(sl, &key);
    
    result = (int*)skiplist_get(sl, &key);
    EXPECT_TRUE(result == NULL);
    
    skiplist_free(sl);
}

void test_skiplist_multiple_inserts() {
    TEST(Skiplist_MultipleInserts);
    skiplist_t* sl = skiplist_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9, 4, 6, 2, 8};
    int values[] = {50, 30, 70, 10, 90, 40, 60, 20, 80};
    
    for (int i = 0; i < 9; i++) {
        skiplist_insert(sl, &keys[i], &values[i]);
    }
    
    for (int i = 0; i < 9; i++) {
        int* result = (int*)skiplist_get(sl, &keys[i]);
        EXPECT_TRUE(result != NULL);
        EXPECT_EQ(*result, values[i]);
    }
    
    skiplist_free(sl);
}

void test_skiplist_update_existing() {
    TEST(Skiplist_UpdateExisting);
    skiplist_t* sl = skiplist_create(int_compare);
    
    int key = 10;
    int value1 = 100;
    int value2 = 200;
    
    skiplist_insert(sl, &key, &value1);
    skiplist_insert(sl, &key, &value2);
    
    int* result = (int*)skiplist_get(sl, &key);
    EXPECT_EQ(*result, 200);
    
    skiplist_free(sl);
}

void test_skiplist_stress() {
    TEST(Skiplist_Stress);
    skiplist_t* sl = skiplist_create(int_compare);
    
    int* keys = malloc(1000 * sizeof(int));
    int* values = malloc(1000 * sizeof(int));
    
    for (int i = 0; i < 1000; i++) {
        keys[i] = i;
        values[i] = i * 10;
        skiplist_insert(sl, &keys[i], &values[i]);
    }
    
    for (int i = 0; i < 1000; i++) {
        int* result = (int*)skiplist_get(sl, &keys[i]);
        EXPECT_TRUE(result != NULL);
        EXPECT_EQ(*result, values[i]);
    }
    
    free(keys);
    free(values);
    skiplist_free(sl);
}

void test_skiplist_free_null() {
    TEST(Skiplist_FreeNull);
    skiplist_free(NULL);
}

int main() {
    test_skiplist_create();
    test_skiplist_insert_get();
    test_skiplist_get_not_found();
    test_skiplist_delete();
    test_skiplist_multiple_inserts();
    test_skiplist_update_existing();
    test_skiplist_stress();
    test_skiplist_free_null();

    return 0;
}
