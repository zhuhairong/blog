#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bplus_tree.h"

static int int_compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void test_bplus_tree_create_free() {
    TEST(BplusTree_CreateFree);
    bplus_tree_t* tree = bplus_tree_create(int_compare);
    EXPECT_TRUE(tree != NULL);
    
    bplus_tree_free(tree);
}

void test_bplus_tree_insert_get() {
    TEST(BplusTree_InsertGet);
    bplus_tree_t* tree = bplus_tree_create(int_compare);
    EXPECT_TRUE(tree != NULL);
    
    int key = 42;
    int value = 100;
    
    bool result = bplus_tree_insert(tree, &key, &value);
    EXPECT_TRUE(result);
    
    void* found = bplus_tree_get(tree, &key);
    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(*(int*)found, value);
    
    bplus_tree_free(tree);
}

void test_bplus_tree_delete() {
    TEST(BplusTree_Delete);
    bplus_tree_t* tree = bplus_tree_create(int_compare);
    EXPECT_TRUE(tree != NULL);
    
    int key = 42;
    int value = 100;
    
    bplus_tree_insert(tree, &key, &value);
    
    bool result = bplus_tree_delete(tree, &key);
    EXPECT_TRUE(result);
    
    void* found = bplus_tree_get(tree, &key);
    EXPECT_TRUE(found == NULL);
    
    bplus_tree_free(tree);
}

void test_bplus_tree_size() {
    TEST(BplusTree_Size);
    bplus_tree_t* tree = bplus_tree_create(int_compare);
    EXPECT_TRUE(tree != NULL);
    
    EXPECT_TRUE(bplus_tree_is_empty(tree));
    
    int key1 = 1, key2 = 2, key3 = 3;
    int val1 = 10, val2 = 20, val3 = 30;
    
    bplus_tree_insert(tree, &key1, &val1);
    bplus_tree_insert(tree, &key2, &val2);
    bplus_tree_insert(tree, &key3, &val3);
    
    EXPECT_EQ(bplus_tree_size(tree), (size_t)3);
    
    bplus_tree_free(tree);
}

void test_bplus_tree_iterator() {
    TEST(BplusTree_Iterator);
    bplus_tree_t* tree = bplus_tree_create(int_compare);
    EXPECT_TRUE(tree != NULL);
    
    int key1 = 1, key2 = 2;
    int val1 = 10, val2 = 20;
    
    bplus_tree_insert(tree, &key1, &val1);
    bplus_tree_insert(tree, &key2, &val2);
    
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    EXPECT_TRUE(iter != NULL);
    
    const void* k;
    void* v;
    int count = 0;
    while (bplus_iterator_next(iter, &k, &v)) {
        count++;
    }
    EXPECT_EQ(count, 2);
    
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

int main() {
    test_bplus_tree_create_free();
    test_bplus_tree_insert_get();
    test_bplus_tree_delete();
    test_bplus_tree_size();
    test_bplus_tree_iterator();

    return 0;
}
