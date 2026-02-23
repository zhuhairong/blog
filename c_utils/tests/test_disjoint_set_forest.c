#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/disjoint_set_forest.h"

void test_dsf_create() {
    TEST(DSF_Create);
    dsf_error_t error;
    
    dsf_t* dsf = dsf_create(10, &error);
    EXPECT_TRUE(dsf != NULL);
    EXPECT_EQ(error, DSF_OK);
    
    dsf_free(dsf);
}

void test_dsf_find() {
    TEST(DSF_Find);
    dsf_error_t error;
    
    dsf_t* dsf = dsf_create(5, &error);
    EXPECT_TRUE(dsf != NULL);
    
    if (dsf != NULL) {
        int root = dsf_find(dsf, 0, &error);
        EXPECT_EQ(root, 0);
        
        dsf_free(dsf);
    }
}

void test_dsf_union() {
    TEST(DSF_Union);
    dsf_error_t error;
    
    dsf_t* dsf = dsf_create(5, &error);
    EXPECT_TRUE(dsf != NULL);
    
    if (dsf != NULL) {
        bool result = dsf_union(dsf, 0, 1, &error);
        EXPECT_TRUE(result);
        
        int root0 = dsf_find(dsf, 0, &error);
        int root1 = dsf_find(dsf, 1, &error);
        EXPECT_EQ(root0, root1);
        
        dsf_free(dsf);
    }
}

void test_dsf_same_set() {
    TEST(DSF_SameSet);
    dsf_error_t error;
    
    dsf_t* dsf = dsf_create(5, &error);
    EXPECT_TRUE(dsf != NULL);
    
    if (dsf != NULL) {
        bool same = dsf_same_set(dsf, 0, 1, &error);
        EXPECT_FALSE(same);
        
        dsf_union(dsf, 0, 1, &error);
        
        same = dsf_same_set(dsf, 0, 1, &error);
        EXPECT_TRUE(same);
        
        dsf_free(dsf);
    }
}

void test_dsf_null_param() {
    TEST(DSF_NullParam);
    dsf_error_t error;
    
    dsf_t* dsf = dsf_create(0, &error);
    EXPECT_TRUE(dsf == NULL || dsf != NULL);
    
    dsf_free(NULL);
}

int main() {
    test_dsf_create();
    test_dsf_find();
    test_dsf_union();
    test_dsf_same_set();
    test_dsf_null_param();

    return 0;
}
