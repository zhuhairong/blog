#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/avl.h"

static int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

static int visit_count = 0;

static void count_visit(void *key, void *value, void *user_data) {
    (void)key;
    (void)value;
    (void)user_data;
    visit_count++;
}

void test_avl_create() {
    TEST(Avl_Create);
    avl_t* t = avl_create(int_compare);
    EXPECT_TRUE(t != NULL);
    EXPECT_EQ(avl_size(t), (size_t)0);
    EXPECT_TRUE(avl_is_empty(t));
    avl_free(t);
}

void test_avl_insert() {
    TEST(Avl_Insert);
    avl_t* t = avl_create(int_compare);
    
    int key = 10;
    int value = 100;
    avl_insert(t, &key, &value);
    
    EXPECT_EQ(avl_size(t), (size_t)1);
    EXPECT_FALSE(avl_is_empty(t));
    
    avl_free(t);
}

void test_avl_get() {
    TEST(Avl_Get);
    avl_t* t = avl_create(int_compare);
    
    int key = 10;
    int value = 100;
    avl_insert(t, &key, &value);
    
    int* result = (int*)avl_get(t, &key);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*result, 100);
    
    avl_free(t);
}

void test_avl_get_not_found() {
    TEST(Avl_GetNotFound);
    avl_t* t = avl_create(int_compare);
    
    int key1 = 10;
    int value = 100;
    avl_insert(t, &key1, &value);
    
    int key2 = 20;
    void* result = avl_get(t, &key2);
    EXPECT_TRUE(result == NULL);
    
    avl_free(t);
}

void test_avl_contains() {
    TEST(Avl_Contains);
    avl_t* t = avl_create(int_compare);
    
    int key = 10;
    int value = 100;
    avl_insert(t, &key, &value);
    
    EXPECT_TRUE(avl_contains(t, &key));
    
    int key2 = 20;
    EXPECT_FALSE(avl_contains(t, &key2));
    
    avl_free(t);
}

void test_avl_delete() {
    TEST(Avl_Delete);
    avl_t* t = avl_create(int_compare);
    
    int key = 10;
    int value = 100;
    avl_insert(t, &key, &value);
    
    EXPECT_TRUE(avl_contains(t, &key));
    
    avl_delete(t, &key);
    
    EXPECT_FALSE(avl_contains(t, &key));
    EXPECT_EQ(avl_size(t), (size_t)0);
    
    avl_free(t);
}

void test_avl_multiple_inserts() {
    TEST(Avl_MultipleInserts);
    avl_t* t = avl_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9, 4, 6, 2, 8};
    int values[] = {50, 30, 70, 10, 90, 40, 60, 20, 80};
    
    for (int i = 0; i < 9; i++) {
        avl_insert(t, &keys[i], &values[i]);
    }
    
    EXPECT_EQ(avl_size(t), (size_t)9);
    
    for (int i = 0; i < 9; i++) {
        EXPECT_TRUE(avl_contains(t, &keys[i]));
        int* result = (int*)avl_get(t, &keys[i]);
        EXPECT_EQ(*result, values[i]);
    }
    
    avl_free(t);
}

void test_avl_min_max() {
    TEST(Avl_MinMax);
    avl_t* t = avl_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9};
    int values[] = {50, 30, 70, 10, 90};
    
    for (int i = 0; i < 5; i++) {
        avl_insert(t, &keys[i], &values[i]);
    }
    
    int* min = (int*)avl_min(t);
    int* max = (int*)avl_max(t);
    
    EXPECT_TRUE(min != NULL);
    EXPECT_TRUE(max != NULL);
    
    avl_free(t);
}

void test_avl_height() {
    TEST(Avl_Height);
    avl_t* t = avl_create(int_compare);
    
    EXPECT_EQ(avl_height(t), 0);
    
    int key = 10;
    int value = 100;
    avl_insert(t, &key, &value);
    
    int height = avl_height(t);
    EXPECT_TRUE(height > 0);
    
    avl_free(t);
}

void test_avl_inorder() {
    TEST(Avl_Inorder);
    avl_t* t = avl_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9};
    int values[] = {50, 30, 70, 10, 90};
    
    for (int i = 0; i < 5; i++) {
        avl_insert(t, &keys[i], &values[i]);
    }
    
    visit_count = 0;
    avl_inorder(t, count_visit, NULL);
    EXPECT_EQ(visit_count, 5);
    
    avl_free(t);
}

void test_avl_preorder() {
    TEST(Avl_Preorder);
    avl_t* t = avl_create(int_compare);
    
    int keys[] = {5, 3, 7};
    int values[] = {50, 30, 70};
    
    for (int i = 0; i < 3; i++) {
        avl_insert(t, &keys[i], &values[i]);
    }
    
    visit_count = 0;
    avl_preorder(t, count_visit, NULL);
    EXPECT_EQ(visit_count, 3);
    
    avl_free(t);
}

void test_avl_postorder() {
    TEST(Avl_Postorder);
    avl_t* t = avl_create(int_compare);
    
    int keys[] = {5, 3, 7};
    int values[] = {50, 30, 70};
    
    for (int i = 0; i < 3; i++) {
        avl_insert(t, &keys[i], &values[i]);
    }
    
    visit_count = 0;
    avl_postorder(t, count_visit, NULL);
    EXPECT_EQ(visit_count, 3);
    
    avl_free(t);
}

void test_avl_iter() {
    TEST(Avl_Iter);
    avl_t* t = avl_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9};
    int values[] = {50, 30, 70, 10, 90};
    
    for (int i = 0; i < 5; i++) {
        avl_insert(t, &keys[i], &values[i]);
    }
    
    avl_iter_t iter = avl_iter_begin(t);
    int count = 0;
    while (avl_iter_valid(&iter)) {
        count++;
        avl_iter_next(&iter);
    }
    EXPECT_EQ(count, 5);
    
    avl_iter_free(&iter);
    avl_free(t);
}

void test_avl_free_null() {
    TEST(Avl_FreeNull);
    avl_free(NULL);
}

void test_avl_stress() {
    TEST(Avl_Stress);
    avl_t* t = avl_create(int_compare);
    
    int* keys = malloc(1000 * sizeof(int));
    int* values = malloc(1000 * sizeof(int));
    
    for (int i = 0; i < 1000; i++) {
        keys[i] = i;
        values[i] = i * 10;
        avl_insert(t, &keys[i], &values[i]);
    }
    
    EXPECT_EQ(avl_size(t), (size_t)1000);
    
    for (int i = 0; i < 1000; i++) {
        EXPECT_TRUE(avl_contains(t, &keys[i]));
    }
    
    free(keys);
    free(values);
    avl_free(t);
}

int main() {
    test_avl_create();
    test_avl_insert();
    test_avl_get();
    test_avl_get_not_found();
    test_avl_contains();
    test_avl_delete();
    test_avl_multiple_inserts();
    test_avl_min_max();
    test_avl_height();
    test_avl_inorder();
    test_avl_preorder();
    test_avl_postorder();
    test_avl_iter();
    test_avl_free_null();
    test_avl_stress();

    return 0;
}
