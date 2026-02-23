#include "../c_utils/utest.h"
#include "../c_utils/hashmap.h"
#include <string.h>

void test_hashmap_create() {
    TEST(Hashmap_Create);
    hashmap_t* m = hashmap_create();
    EXPECT_TRUE(m != NULL);
    EXPECT_TRUE(hashmap_is_empty(m));
    hashmap_free(m);
}

void test_hashmap_set_get() {
    TEST(Hashmap_SetGet);
    hashmap_t* m = hashmap_create();
    EXPECT_TRUE(m != NULL);
    
    int value = 42;
    EXPECT_TRUE(hashmap_set(m, "key", &value));
    
    int* retrieved = (int*)hashmap_get(m, "key");
    EXPECT_TRUE(retrieved != NULL);
    EXPECT_EQ(*retrieved, 42);
    
    hashmap_free(m);
}

void test_hashmap_remove() {
    TEST(Hashmap_Remove);
    hashmap_t* m = hashmap_create();
    EXPECT_TRUE(m != NULL);
    
    int value = 42;
    hashmap_set(m, "key", &value);
    
    EXPECT_TRUE(hashmap_remove(m, "key"));
    EXPECT_TRUE(hashmap_get(m, "key") == NULL);
    
    hashmap_free(m);
}

void test_hashmap_size() {
    TEST(Hashmap_Size);
    hashmap_t* m = hashmap_create();
    EXPECT_TRUE(m != NULL);
    
    EXPECT_EQ(hashmap_size(m), 0);
    
    int v1 = 1, v2 = 2, v3 = 3;
    hashmap_set(m, "a", &v1);
    EXPECT_EQ(hashmap_size(m), 1);
    
    hashmap_set(m, "b", &v2);
    EXPECT_EQ(hashmap_size(m), 2);
    
    hashmap_set(m, "c", &v3);
    EXPECT_EQ(hashmap_size(m), 3);
    
    hashmap_free(m);
}

void test_hashmap_clear() {
    TEST(Hashmap_Clear);
    hashmap_t* m = hashmap_create();
    EXPECT_TRUE(m != NULL);
    
    int v1 = 1, v2 = 2;
    hashmap_set(m, "a", &v1);
    hashmap_set(m, "b", &v2);
    
    hashmap_clear(m);
    EXPECT_EQ(hashmap_size(m), 0);
    EXPECT_TRUE(hashmap_is_empty(m));
    
    hashmap_free(m);
}

void test_hashmap_update() {
    TEST(Hashmap_Update);
    hashmap_t* m = hashmap_create();
    EXPECT_TRUE(m != NULL);
    
    int v1 = 1, v2 = 2;
    hashmap_set(m, "key", &v1);
    
    int* retrieved = (int*)hashmap_get(m, "key");
    EXPECT_EQ(*retrieved, 1);
    
    hashmap_set(m, "key", &v2);
    retrieved = (int*)hashmap_get(m, "key");
    EXPECT_EQ(*retrieved, 2);
    
    hashmap_free(m);
}

int main() {
    UTEST_BEGIN();
    test_hashmap_create();
    test_hashmap_set_get();
    test_hashmap_remove();
    test_hashmap_size();
    test_hashmap_clear();
    test_hashmap_update();
    UTEST_END();
}
