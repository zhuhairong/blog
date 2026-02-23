#include "../c_utils/utest.h"
#include "../c_utils/list.h"
#include <string.h>

void test_list_create() {
    TEST(List_Create);
    list_t* l = list_create();
    EXPECT_TRUE(l != NULL);
    EXPECT_TRUE(list_is_empty(l));
    EXPECT_EQ(list_size(l), 0);
    list_free(l);
}

void test_list_push_pop() {
    TEST(List_PushPop);
    list_t* l = list_create();
    EXPECT_TRUE(l != NULL);
    
    int a = 1, b = 2, c = 3;
    list_push_back(l, &a);
    list_push_back(l, &b);
    list_push_back(l, &c);
    
    EXPECT_EQ(list_size(l), 3);
    
    int* front = (int*)list_front(l);
    EXPECT_EQ(*front, 1);
    
    int* back = (int*)list_back(l);
    EXPECT_EQ(*back, 3);
    
    int* popped = (int*)list_pop_front(l);
    EXPECT_EQ(*popped, 1);
    EXPECT_EQ(list_size(l), 2);
    
    list_free(l);
}

void test_list_push_front() {
    TEST(List_PushFront);
    list_t* l = list_create();
    EXPECT_TRUE(l != NULL);
    
    int a = 1, b = 2, c = 3;
    list_push_front(l, &a);
    list_push_front(l, &b);
    list_push_front(l, &c);
    
    EXPECT_EQ(list_size(l), 3);
    
    int* front = (int*)list_front(l);
    EXPECT_EQ(*front, 3);
    
    int* back = (int*)list_back(l);
    EXPECT_EQ(*back, 1);
    
    list_free(l);
}

void test_list_insert_remove() {
    TEST(List_InsertRemove);
    list_t* l = list_create();
    EXPECT_TRUE(l != NULL);
    
    int a = 1, b = 2, d = 4;
    list_push_back(l, &a);
    list_push_back(l, &b);
    list_push_back(l, &d);
    
    int c = 3;
    EXPECT_TRUE(list_insert(l, 2, &c));
    EXPECT_EQ(list_size(l), 4);
    
    int* val = (int*)list_at(l, 2);
    EXPECT_EQ(*val, 3);
    
    EXPECT_TRUE(list_remove(l, 2));
    EXPECT_EQ(list_size(l), 3);
    
    list_free(l);
}

void test_list_at() {
    TEST(List_At);
    list_t* l = list_create();
    EXPECT_TRUE(l != NULL);
    
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        list_push_back(l, &values[i]);
    }
    
    for (int i = 0; i < 5; i++) {
        int* val = (int*)list_at(l, i);
        EXPECT_TRUE(val != NULL);
        EXPECT_EQ(*val, values[i]);
    }
    
    list_free(l);
}

void test_list_clear() {
    TEST(List_Clear);
    list_t* l = list_create();
    EXPECT_TRUE(l != NULL);
    
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        list_push_back(l, &values[i]);
    }
    
    list_clear(l);
    EXPECT_EQ(list_size(l), 0);
    EXPECT_TRUE(list_is_empty(l));
    
    list_free(l);
}

void test_list_reverse() {
    TEST(List_Reverse);
    list_t* l = list_create();
    EXPECT_TRUE(l != NULL);
    
    int values[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        list_push_back(l, &values[i]);
    }
    
    list_reverse(l);
    
    int* front = (int*)list_front(l);
    EXPECT_EQ(*front, 3);
    
    int* back = (int*)list_back(l);
    EXPECT_EQ(*back, 1);
    
    list_free(l);
}

int main() {
    UTEST_BEGIN();
    test_list_create();
    test_list_push_pop();
    test_list_push_front();
    test_list_insert_remove();
    test_list_at();
    test_list_clear();
    test_list_reverse();
    UTEST_END();
}
