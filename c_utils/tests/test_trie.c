#include "../c_utils/utest.h"
#include "../c_utils/trie.h"
#include <string.h>

void test_trie_create() {
    TEST(Trie_Create);
    trie_t* t = trie_create();
    EXPECT_TRUE(t != NULL);
    EXPECT_EQ(t->size, 0);
    trie_free(t);
}

void test_trie_insert_get() {
    TEST(Trie_InsertGet);
    trie_t* t = trie_create();
    EXPECT_TRUE(t != NULL);
    
    int value1 = 1, value2 = 2;
    EXPECT_EQ(trie_insert(t, "hello", &value1), TRIE_OK);
    EXPECT_EQ(trie_insert(t, "world", &value2), TRIE_OK);
    
    int* found = (int*)trie_get(t, "hello");
    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(*found, 1);
    
    found = (int*)trie_get(t, "world");
    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(*found, 2);
    
    found = (int*)trie_get(t, "notfound");
    EXPECT_TRUE(found == NULL);
    
    trie_free(t);
}

void test_trie_prefix() {
    TEST(Trie_Prefix);
    trie_t* t = trie_create();
    EXPECT_TRUE(t != NULL);
    
    int v1 = 1, v2 = 2, v3 = 3;
    trie_insert(t, "apple", &v1);
    trie_insert(t, "app", &v2);
    trie_insert(t, "application", &v3);
    
    EXPECT_TRUE(trie_get(t, "app") != NULL);
    EXPECT_TRUE(trie_get(t, "apple") != NULL);
    EXPECT_TRUE(trie_get(t, "application") != NULL);
    EXPECT_TRUE(trie_get(t, "banana") == NULL);
    
    trie_free(t);
}

void test_trie_empty_key() {
    TEST(Trie_EmptyKey);
    trie_t* t = trie_create();
    EXPECT_TRUE(t != NULL);
    
    int value = 42;
    trie_error_t err = trie_insert(t, "", &value);
    EXPECT_EQ(err, TRIE_EMPTY_KEY);
    
    trie_free(t);
}

void test_trie_clear() {
    TEST(Trie_Clear);
    trie_t* t = trie_create();
    EXPECT_TRUE(t != NULL);
    
    int v1 = 1, v2 = 2;
    trie_insert(t, "a", &v1);
    trie_insert(t, "b", &v2);
    
    trie_clear(t);
    EXPECT_EQ(trie_size(t), 0);
    
    trie_free(t);
}

int main() {
    UTEST_BEGIN();
    test_trie_create();
    test_trie_insert_get();
    test_trie_prefix();
    test_trie_empty_key();
    test_trie_clear();
    UTEST_END();
}
