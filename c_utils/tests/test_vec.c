#include "../c_utils/utest.h"
#include "../c_utils/vec.h"
#include <string.h>

void test_vec_create() {
    TEST(Vec_Create);
    vec_t* v = vec_create(sizeof(int));
    EXPECT_TRUE(v != NULL);
    EXPECT_TRUE(vec_is_empty(v));
    EXPECT_EQ(vec_size(v), 0);
    vec_free(v);
}

void test_vec_push_pop() {
    TEST(Vec_PushPop);
    vec_t* v = vec_create(sizeof(int));
    EXPECT_TRUE(v != NULL);
    
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(vec_push(v, &values[i]));
    }
    
    EXPECT_EQ(vec_size(v), 3);
    
    int out;
    EXPECT_TRUE(vec_pop(v, &out));
    EXPECT_EQ(out, 3);
    EXPECT_EQ(vec_size(v), 2);
    
    vec_free(v);
}

void test_vec_at() {
    TEST(Vec_At);
    vec_t* v = vec_create(sizeof(int));
    EXPECT_TRUE(v != NULL);
    
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        vec_push(v, &values[i]);
    }
    
    int* p = (int*)vec_at(v, 0);
    EXPECT_TRUE(p != NULL);
    EXPECT_EQ(*p, 10);
    
    p = (int*)vec_at(v, 2);
    EXPECT_TRUE(p != NULL);
    EXPECT_EQ(*p, 30);
    
    vec_free(v);
}

void test_vec_insert_remove() {
    TEST(Vec_InsertRemove);
    vec_t* v = vec_create(sizeof(int));
    EXPECT_TRUE(v != NULL);
    
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        vec_push(v, &values[i]);
    }
    
    int insert_val = 99;
    EXPECT_TRUE(vec_insert(v, 1, &insert_val));
    EXPECT_EQ(vec_size(v), 4);
    
    int* p = (int*)vec_at(v, 1);
    EXPECT_EQ(*p, 99);
    
    EXPECT_TRUE(vec_remove(v, 1));
    EXPECT_EQ(vec_size(v), 3);
    
    p = (int*)vec_at(v, 1);
    EXPECT_EQ(*p, 2);
    
    vec_free(v);
}

void test_vec_clear() {
    TEST(Vec_Clear);
    vec_t* v = vec_create(sizeof(int));
    EXPECT_TRUE(v != NULL);
    
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        vec_push(v, &values[i]);
    }
    
    vec_clear(v);
    EXPECT_EQ(vec_size(v), 0);
    EXPECT_TRUE(vec_is_empty(v));
    
    vec_free(v);
}

void test_vec_capacity() {
    TEST(Vec_Capacity);
    vec_t* v = vec_create(sizeof(int));
    EXPECT_TRUE(v != NULL);
    
    EXPECT_TRUE(vec_capacity(v) >= 0);
    
    for (int i = 0; i < 100; i++) {
        vec_push(v, &i);
    }
    
    EXPECT_TRUE(vec_capacity(v) >= 100);
    
    vec_free(v);
}

int main() {
    UTEST_BEGIN();
    test_vec_create();
    test_vec_push_pop();
    test_vec_at();
    test_vec_insert_remove();
    test_vec_clear();
    test_vec_capacity();
    UTEST_END();
}
