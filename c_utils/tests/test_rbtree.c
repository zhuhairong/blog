#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/rbtree.h"

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

void test_rbtree_create() {
    TEST(Rbtree_Create);
    rbtree_t* t = rbtree_create(int_compare);
    EXPECT_TRUE(t != NULL);
    EXPECT_EQ(rbtree_size(t), (size_t)0);
    EXPECT_TRUE(rbtree_is_empty(t));
    rbtree_free(t);
}

void test_rbtree_insert() {
    TEST(Rbtree_Insert);
    rbtree_t* t = rbtree_create(int_compare);
    
    int key = 10;
    int value = 100;
    rbtree_insert(t, &key, &value);
    
    EXPECT_EQ(rbtree_size(t), (size_t)1);
    EXPECT_FALSE(rbtree_is_empty(t));
    
    rbtree_free(t);
}

void test_rbtree_get() {
    TEST(Rbtree_Get);
    rbtree_t* t = rbtree_create(int_compare);
    
    int key = 10;
    int value = 100;
    rbtree_insert(t, &key, &value);
    
    int* result = (int*)rbtree_get(t, &key);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*result, 100);
    
    rbtree_free(t);
}

void test_rbtree_get_not_found() {
    TEST(Rbtree_GetNotFound);
    rbtree_t* t = rbtree_create(int_compare);
    
    int key1 = 10;
    int value = 100;
    rbtree_insert(t, &key1, &value);
    
    int key2 = 20;
    void* result = rbtree_get(t, &key2);
    EXPECT_TRUE(result == NULL);
    
    rbtree_free(t);
}

void test_rbtree_contains() {
    TEST(Rbtree_Contains);
    rbtree_t* t = rbtree_create(int_compare);
    
    int key = 10;
    int value = 100;
    rbtree_insert(t, &key, &value);
    
    EXPECT_TRUE(rbtree_contains(t, &key));
    
    int key2 = 20;
    EXPECT_FALSE(rbtree_contains(t, &key2));
    
    rbtree_free(t);
}

void test_rbtree_delete() {
    TEST(Rbtree_Delete);
    rbtree_t* t = rbtree_create(int_compare);
    
    int key = 10;
    int value = 100;
    rbtree_insert(t, &key, &value);
    
    EXPECT_TRUE(rbtree_contains(t, &key));
    
    rbtree_delete(t, &key);
    
    EXPECT_FALSE(rbtree_contains(t, &key));
    EXPECT_EQ(rbtree_size(t), (size_t)0);
    
    rbtree_free(t);
}

void test_rbtree_multiple_inserts() {
    TEST(Rbtree_MultipleInserts);
    rbtree_t* t = rbtree_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9, 4, 6, 2, 8};
    int values[] = {50, 30, 70, 10, 90, 40, 60, 20, 80};
    
    for (int i = 0; i < 9; i++) {
        rbtree_insert(t, &keys[i], &values[i]);
    }
    
    EXPECT_EQ(rbtree_size(t), (size_t)9);
    
    for (int i = 0; i < 9; i++) {
        EXPECT_TRUE(rbtree_contains(t, &keys[i]));
        int* result = (int*)rbtree_get(t, &keys[i]);
        EXPECT_EQ(*result, values[i]);
    }
    
    rbtree_free(t);
}

void test_rbtree_min_max() {
    TEST(Rbtree_MinMax);
    rbtree_t* t = rbtree_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9};
    int values[] = {50, 30, 70, 10, 90};
    
    for (int i = 0; i < 5; i++) {
        rbtree_insert(t, &keys[i], &values[i]);
    }
    
    int* min = (int*)rbtree_min(t);
    int* max = (int*)rbtree_max(t);
    
    EXPECT_TRUE(min != NULL);
    EXPECT_TRUE(max != NULL);
    
    rbtree_free(t);
}

void test_rbtree_height() {
    TEST(Rbtree_Height);
    rbtree_t* t = rbtree_create(int_compare);
    
    EXPECT_EQ(rbtree_height(t), 0);
    
    int key = 10;
    int value = 100;
    rbtree_insert(t, &key, &value);
    
    int height = rbtree_height(t);
    EXPECT_TRUE(height > 0);
    
    rbtree_free(t);
}

void test_rbtree_inorder() {
    TEST(Rbtree_Inorder);
    rbtree_t* t = rbtree_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9};
    int values[] = {50, 30, 70, 10, 90};
    
    for (int i = 0; i < 5; i++) {
        rbtree_insert(t, &keys[i], &values[i]);
    }
    
    visit_count = 0;
    rbtree_inorder(t, count_visit, NULL);
    EXPECT_EQ(visit_count, 5);
    
    rbtree_free(t);
}

void test_rbtree_preorder() {
    TEST(Rbtree_Preorder);
    rbtree_t* t = rbtree_create(int_compare);
    
    int keys[] = {5, 3, 7};
    int values[] = {50, 30, 70};
    
    for (int i = 0; i < 3; i++) {
        rbtree_insert(t, &keys[i], &values[i]);
    }
    
    visit_count = 0;
    rbtree_preorder(t, count_visit, NULL);
    EXPECT_EQ(visit_count, 3);
    
    rbtree_free(t);
}

void test_rbtree_postorder() {
    TEST(Rbtree_Postorder);
    rbtree_t* t = rbtree_create(int_compare);
    
    int keys[] = {5, 3, 7};
    int values[] = {50, 30, 70};
    
    for (int i = 0; i < 3; i++) {
        rbtree_insert(t, &keys[i], &values[i]);
    }
    
    visit_count = 0;
    rbtree_postorder(t, count_visit, NULL);
    EXPECT_EQ(visit_count, 3);
    
    rbtree_free(t);
}

void test_rbtree_iter() {
    TEST(Rbtree_Iter);
    rbtree_t* t = rbtree_create(int_compare);
    
    int keys[] = {5, 3, 7, 1, 9};
    int values[] = {50, 30, 70, 10, 90};
    
    for (int i = 0; i < 5; i++) {
        rbtree_insert(t, &keys[i], &values[i]);
    }
    
    rbtree_iter_t iter = rbtree_iter_begin(t);
    int count = 0;
    while (rbtree_iter_valid(&iter)) {
        count++;
        rbtree_iter_next(&iter);
    }
    EXPECT_EQ(count, 5);
    
    rbtree_iter_free(&iter);
    rbtree_free(t);
}

void test_rbtree_free_null() {
    TEST(Rbtree_FreeNull);
    rbtree_free(NULL);
}

void test_rbtree_stress() {
    TEST(Rbtree_Stress);
    rbtree_t* t = rbtree_create(int_compare);
    
    int* keys = malloc(1000 * sizeof(int));
    int* values = malloc(1000 * sizeof(int));
    
    for (int i = 0; i < 1000; i++) {
        keys[i] = i;
        values[i] = i * 10;
        rbtree_insert(t, &keys[i], &values[i]);
    }
    
    EXPECT_EQ(rbtree_size(t), (size_t)1000);
    
    for (int i = 0; i < 1000; i++) {
        EXPECT_TRUE(rbtree_contains(t, &keys[i]));
    }
    
    free(keys);
    free(values);
    rbtree_free(t);
}

int main() {
    test_rbtree_create();
    test_rbtree_insert();
    test_rbtree_get();
    test_rbtree_get_not_found();
    test_rbtree_contains();
    test_rbtree_delete();
    test_rbtree_multiple_inserts();
    test_rbtree_min_max();
    test_rbtree_height();
    test_rbtree_inorder();
    test_rbtree_preorder();
    test_rbtree_postorder();
    test_rbtree_iter();
    test_rbtree_free_null();
    test_rbtree_stress();

    return 0;
}
