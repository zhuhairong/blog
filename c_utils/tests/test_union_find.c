#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/utest.h"
#include "../c_utils/union_find.h"

void test_uf_create() {
    TEST(UF_Create);
    union_find_t* uf = uf_create(10);
    EXPECT_TRUE(uf != NULL);
    EXPECT_TRUE(uf->parent != NULL);
    EXPECT_TRUE(uf->rank != NULL);
    EXPECT_EQ(uf->n, (size_t)10);
    uf_free(uf);
}

void test_uf_create_zero() {
    TEST(UF_CreateZero);
    union_find_t* uf = uf_create(0);
    EXPECT_TRUE(uf == NULL);
}

void test_uf_create_large() {
    TEST(UF_CreateLarge);
    union_find_t* uf = uf_create(10000);
    EXPECT_TRUE(uf != NULL);
    EXPECT_EQ(uf->n, (size_t)10000);
    uf_free(uf);
}

void test_uf_find_initial() {
    TEST(UF_FindInitial);
    union_find_t* uf = uf_create(5);
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(uf_find(uf, i), i);
    }
    uf_free(uf);
}

void test_uf_union_basic() {
    TEST(UF_UnionBasic);
    union_find_t* uf = uf_create(5);
    
    uf_union(uf, 0, 1);
    EXPECT_EQ(uf_find(uf, 0), uf_find(uf, 1));
    
    uf_union(uf, 2, 3);
    EXPECT_EQ(uf_find(uf, 2), uf_find(uf, 3));
    
    EXPECT_TRUE(uf_find(uf, 0) != uf_find(uf, 2));
    
    uf_free(uf);
}

void test_uf_connected() {
    TEST(UF_Connected);
    union_find_t* uf = uf_create(5);
    
    EXPECT_FALSE(uf_connected(uf, 0, 1));
    EXPECT_FALSE(uf_connected(uf, 2, 3));
    
    uf_union(uf, 0, 1);
    EXPECT_TRUE(uf_connected(uf, 0, 1));
    
    uf_union(uf, 2, 3);
    EXPECT_TRUE(uf_connected(uf, 2, 3));
    EXPECT_FALSE(uf_connected(uf, 0, 2));
    
    uf_free(uf);
}

void test_uf_union_transitive() {
    TEST(UF_UnionTransitive);
    union_find_t* uf = uf_create(5);
    
    uf_union(uf, 0, 1);
    uf_union(uf, 1, 2);
    uf_union(uf, 2, 3);
    uf_union(uf, 3, 4);
    
    EXPECT_TRUE(uf_connected(uf, 0, 4));
    EXPECT_TRUE(uf_connected(uf, 1, 3));
    EXPECT_TRUE(uf_connected(uf, 0, 2));
    
    uf_free(uf);
}

void test_uf_union_same_element() {
    TEST(UF_UnionSameElement);
    union_find_t* uf = uf_create(5);
    
    uf_union(uf, 0, 0);
    EXPECT_EQ(uf_find(uf, 0), 0);
    
    uf_free(uf);
}

void test_uf_union_already_connected() {
    TEST(UF_UnionAlreadyConnected);
    union_find_t* uf = uf_create(5);
    
    uf_union(uf, 0, 1);
    int root1 = uf_find(uf, 0);
    
    uf_union(uf, 0, 1);
    int root2 = uf_find(uf, 0);
    
    EXPECT_EQ(root1, root2);
    
    uf_free(uf);
}

void test_uf_path_compression() {
    TEST(UF_PathCompression);
    union_find_t* uf = uf_create(10);
    
    for (int i = 0; i < 9; i++) {
        uf_union(uf, i, i + 1);
    }
    
    for (int i = 0; i < 10; i++) {
        uf_find(uf, i);
    }
    
    int root = uf_find(uf, 0);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(uf_find(uf, i), root);
    }
    
    uf_free(uf);
}

void test_uf_rank_union() {
    TEST(UF_RankUnion);
    union_find_t* uf = uf_create(10);
    
    uf_union(uf, 0, 1);
    uf_union(uf, 2, 3);
    uf_union(uf, 0, 2);
    
    EXPECT_TRUE(uf_connected(uf, 0, 3));
    EXPECT_TRUE(uf_connected(uf, 1, 2));
    
    uf_free(uf);
}

void test_uf_stress_many_unions() {
    TEST(UF_StressManyUnions);
    union_find_t* uf = uf_create(1000);
    
    for (int i = 0; i < 999; i++) {
        uf_union(uf, i, i + 1);
    }
    
    for (int i = 0; i < 1000; i++) {
        EXPECT_TRUE(uf_connected(uf, 0, i));
    }
    
    uf_free(uf);
}

void test_uf_stress_alternating() {
    TEST(UF_StressAlternating);
    union_find_t* uf = uf_create(100);
    
    for (int i = 0; i < 50; i++) {
        uf_union(uf, i, i + 50);
    }
    
    for (int i = 0; i < 50; i++) {
        EXPECT_TRUE(uf_connected(uf, i, i + 50));
    }
    
    for (int i = 1; i < 50; i++) {
        EXPECT_FALSE(uf_connected(uf, 0, i));
    }
    
    uf_free(uf);
}

void test_uf_free_null() {
    TEST(UF_FreeNull);
    uf_free(NULL);
}

void test_uf_multiple_components() {
    TEST(UF_MultipleComponents);
    union_find_t* uf = uf_create(9);
    
    uf_union(uf, 0, 1);
    uf_union(uf, 1, 2);
    
    uf_union(uf, 3, 4);
    uf_union(uf, 4, 5);
    
    uf_union(uf, 6, 7);
    uf_union(uf, 7, 8);
    
    EXPECT_TRUE(uf_connected(uf, 0, 2));
    EXPECT_TRUE(uf_connected(uf, 3, 5));
    EXPECT_TRUE(uf_connected(uf, 6, 8));
    
    EXPECT_FALSE(uf_connected(uf, 0, 3));
    EXPECT_FALSE(uf_connected(uf, 2, 6));
    EXPECT_FALSE(uf_connected(uf, 4, 7));
    
    uf_free(uf);
}

int main() {
    test_uf_create();
    test_uf_create_zero();
    test_uf_create_large();
    test_uf_find_initial();
    test_uf_union_basic();
    test_uf_connected();
    test_uf_union_transitive();
    test_uf_union_same_element();
    test_uf_union_already_connected();
    test_uf_path_compression();
    test_uf_rank_union();
    test_uf_stress_many_unions();
    test_uf_stress_alternating();
    test_uf_free_null();
    test_uf_multiple_components();

    return 0;
}
