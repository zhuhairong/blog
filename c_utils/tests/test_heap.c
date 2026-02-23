#include "../c_utils/utest.h"
#include "../c_utils/heap.h"
#include <string.h>

static int int_compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void test_heap_create_min() {
    TEST(Heap_CreateMin);
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    heap_error_t err;
    
    heap_t* h = heap_create(&config, &err);
    EXPECT_TRUE(h != NULL);
    EXPECT_EQ(err, HEAP_OK);
    EXPECT_TRUE(heap_is_empty(h));
    
    heap_free(h);
}

void test_heap_create_max() {
    TEST(Heap_CreateMax);
    heap_config_t config = heap_default_config(HEAP_TYPE_MAX, sizeof(int), NULL);
    heap_error_t err;
    
    heap_t* h = heap_create(&config, &err);
    EXPECT_TRUE(h != NULL);
    EXPECT_EQ(err, HEAP_OK);
    
    heap_free(h);
}

void test_heap_push_pop_min() {
    TEST(Heap_PushPopMin);
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    heap_error_t err;
    heap_t* h = heap_create(&config, NULL);
    EXPECT_TRUE(h != NULL);
    
    int values[] = {5, 3, 8, 1, 9};
    for (int i = 0; i < 5; i++) {
        heap_push(h, &values[i], &err);
    }
    
    int out;
    heap_pop(h, &out, &err);
    EXPECT_EQ(out, 1);
    
    heap_pop(h, &out, &err);
    EXPECT_EQ(out, 3);
    
    heap_pop(h, &out, &err);
    EXPECT_EQ(out, 5);
    
    heap_free(h);
}

void test_heap_push_pop_max() {
    TEST(Heap_PushPopMax);
    heap_config_t config = heap_default_config(HEAP_TYPE_MAX, sizeof(int), NULL);
    heap_error_t err;
    heap_t* h = heap_create(&config, NULL);
    EXPECT_TRUE(h != NULL);
    
    int values[] = {5, 3, 8, 1, 9};
    for (int i = 0; i < 5; i++) {
        heap_push(h, &values[i], &err);
    }
    
    int out;
    heap_pop(h, &out, &err);
    EXPECT_EQ(out, 9);
    
    heap_pop(h, &out, &err);
    EXPECT_EQ(out, 8);
    
    heap_pop(h, &out, &err);
    EXPECT_EQ(out, 5);
    
    heap_free(h);
}

void test_heap_peek() {
    TEST(Heap_Peek);
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    heap_error_t err;
    heap_t* h = heap_create(&config, NULL);
    EXPECT_TRUE(h != NULL);
    
    int values[] = {5, 3, 8};
    for (int i = 0; i < 3; i++) {
        heap_push(h, &values[i], &err);
    }
    
    int* top = (int*)heap_peek(h, &err);
    EXPECT_TRUE(top != NULL);
    EXPECT_EQ(*top, 3);
    
    EXPECT_EQ(heap_size(h), 3);
    
    heap_free(h);
}

void test_heap_size() {
    TEST(Heap_Size);
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    heap_error_t err;
    heap_t* h = heap_create(&config, NULL);
    EXPECT_TRUE(h != NULL);
    
    EXPECT_EQ(heap_size(h), 0);
    
    int v = 1;
    heap_push(h, &v, &err);
    EXPECT_EQ(heap_size(h), 1);
    
    heap_pop(h, &v, &err);
    EXPECT_EQ(heap_size(h), 0);
    
    heap_free(h);
}

void test_heap_custom_compare() {
    TEST(Heap_CustomCompare);
    heap_config_t config = heap_default_config(HEAP_TYPE_CUSTOM, sizeof(int), int_compare);
    heap_error_t err;
    heap_t* h = heap_create(&config, NULL);
    EXPECT_TRUE(h != NULL);
    
    int values[] = {5, 3, 8, 1};
    for (int i = 0; i < 4; i++) {
        heap_push(h, &values[i], &err);
    }
    
    int out;
    heap_pop(h, &out, &err);
    EXPECT_EQ(out, 1);
    
    heap_free(h);
}

int main() {
    UTEST_BEGIN();
    test_heap_create_min();
    test_heap_create_max();
    test_heap_push_pop_min();
    test_heap_push_pop_max();
    test_heap_peek();
    test_heap_size();
    test_heap_custom_compare();
    UTEST_END();
}
