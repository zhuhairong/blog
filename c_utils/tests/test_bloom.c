#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bloom.h"

void test_bloom_create() {
    TEST(Bloom_Create);
    bloom_t* bloom = bloom_create(1000, 0.01);
    EXPECT_TRUE(bloom != NULL);
    bloom_free(bloom);
}

void test_bloom_create_invalid_params() {
    TEST(Bloom_CreateInvalidParams);
    bloom_t* bloom = bloom_create(0, 0.01);
    EXPECT_TRUE(bloom == NULL);
    
    bloom = bloom_create(1000, 0.0);
    EXPECT_TRUE(bloom == NULL);
    
    bloom = bloom_create(1000, 1.0);
    EXPECT_TRUE(bloom == NULL);
}

void test_bloom_add_and_check() {
    TEST(Bloom_AddAndCheck);
    bloom_t* bloom = bloom_create(100, 0.01);
    EXPECT_TRUE(bloom != NULL);
    
    const char* key = "test_key";
    bool added = bloom_add(bloom, key, strlen(key));
    EXPECT_TRUE(added);
    
    bool exists = bloom_check(bloom, key, strlen(key));
    EXPECT_TRUE(exists);
    
    bloom_free(bloom);
}

void test_bloom_check_nonexistent() {
    TEST(Bloom_CheckNonexistent);
    bloom_t* bloom = bloom_create(100, 0.01);
    
    const char* key1 = "key1";
    bloom_add(bloom, key1, strlen(key1));
    
    const char* key2 = "key2";
    bool exists = bloom_check(bloom, key2, strlen(key2));
    EXPECT_FALSE(exists);
    
    bloom_free(bloom);
}

void test_bloom_add_multiple() {
    TEST(Bloom_AddMultiple);
    bloom_t* bloom = bloom_create(1000, 0.01);
    
    const char* keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int num_keys = 5;
    
    for (int i = 0; i < num_keys; i++) {
        bool added = bloom_add(bloom, keys[i], strlen(keys[i]));
        EXPECT_TRUE(added);
    }
    
    for (int i = 0; i < num_keys; i++) {
        bool exists = bloom_check(bloom, keys[i], strlen(keys[i]));
        EXPECT_TRUE(exists);
    }
    
    bloom_free(bloom);
}

void test_bloom_reset() {
    TEST(Bloom_Reset);
    bloom_t* bloom = bloom_create(100, 0.01);
    
    const char* key = "test_key";
    bloom_add(bloom, key, strlen(key));
    
    bool exists = bloom_check(bloom, key, strlen(key));
    EXPECT_TRUE(exists);
    
    bool reset = bloom_reset(bloom);
    EXPECT_TRUE(reset);
    
    exists = bloom_check(bloom, key, strlen(key));
    EXPECT_FALSE(exists);
    
    bloom_free(bloom);
}

void test_bloom_stats() {
    TEST(Bloom_Stats);
    bloom_t* bloom = bloom_create(100, 0.01);
    
    const char* keys[] = {"a", "b", "c"};
    for (int i = 0; i < 3; i++) {
        bloom_add(bloom, keys[i], strlen(keys[i]));
    }
    
    size_t estimated;
    double fp_rate;
    bool success = bloom_stats(bloom, &estimated, &fp_rate);
    EXPECT_TRUE(success);
    EXPECT_TRUE(estimated > 0);
    
    bloom_free(bloom);
}

void test_bloom_validate_params() {
    TEST(Bloom_ValidateParams);
    EXPECT_TRUE(bloom_validate_params(100, 0.01));
    EXPECT_TRUE(bloom_validate_params(1000, 0.001));
    EXPECT_FALSE(bloom_validate_params(0, 0.01));
    EXPECT_FALSE(bloom_validate_params(100, 0.0));
    EXPECT_FALSE(bloom_validate_params(100, 1.0));
}

void test_bloom_serialize_deserialize() {
    TEST(Bloom_SerializeDeserialize);
    bloom_t* bloom = bloom_create(100, 0.01);
    
    const char* keys[] = {"key1", "key2", "key3"};
    for (int i = 0; i < 3; i++) {
        bloom_add(bloom, keys[i], strlen(keys[i]));
    }
    
    size_t buf_size = 1024;
    uint8_t* buf = (uint8_t*)malloc(buf_size);
    size_t written;
    
    bool serialized = bloom_serialize(bloom, buf, buf_size, &written);
    EXPECT_TRUE(serialized);
    EXPECT_TRUE(written > 0);
    
    bloom_t* bloom2 = bloom_deserialize(buf, written);
    EXPECT_TRUE(bloom2 != NULL);
    
    for (int i = 0; i < 3; i++) {
        bool exists = bloom_check(bloom2, keys[i], strlen(keys[i]));
        EXPECT_TRUE(exists);
    }
    
    free(buf);
    bloom_free(bloom);
    bloom_free(bloom2);
}

void test_bloom_free_null() {
    TEST(Bloom_FreeNull);
    bloom_free(NULL);
}

void test_bloom_add_null_data() {
    TEST(Bloom_AddNullData);
    bloom_t* bloom = bloom_create(100, 0.01);
    
    bool added = bloom_add(bloom, NULL, 0);
    EXPECT_FALSE(added);
    
    bloom_free(bloom);
}

void test_bloom_check_null_data() {
    TEST(Bloom_CheckNullData);
    bloom_t* bloom = bloom_create(100, 0.01);
    
    bool exists = bloom_check(bloom, NULL, 0);
    EXPECT_FALSE(exists);
    
    bloom_free(bloom);
}

void test_bloom_stress_many_elements() {
    TEST(Bloom_StressManyElements);
    bloom_t* bloom = bloom_create(10000, 0.01);
    
    char key[32];
    for (int i = 0; i < 5000; i++) {
        snprintf(key, sizeof(key), "key_%d", i);
        bool added = bloom_add(bloom, key, strlen(key));
        EXPECT_TRUE(added);
    }
    
    int found = 0;
    for (int i = 0; i < 5000; i++) {
        snprintf(key, sizeof(key), "key_%d", i);
        if (bloom_check(bloom, key, strlen(key))) {
            found++;
        }
    }
    EXPECT_EQ(found, 5000);
    
    bloom_free(bloom);
}

void test_bloom_edge_case_single_element() {
    TEST(Bloom_EdgeCaseSingleElement);
    bloom_t* bloom = bloom_create(1, 0.01);
    
    const char* key = "single";
    bloom_add(bloom, key, strlen(key));
    
    bool exists = bloom_check(bloom, key, strlen(key));
    EXPECT_TRUE(exists);
    
    bloom_free(bloom);
}

void test_bloom_edge_case_empty_key() {
    TEST(Bloom_EdgeCaseEmptyKey);
    bloom_t* bloom = bloom_create(100, 0.01);
    
    const char* key = "";
    bool added = bloom_add(bloom, key, 0);
    EXPECT_TRUE(added);
    
    bool exists = bloom_check(bloom, key, 0);
    EXPECT_TRUE(exists);
    
    bloom_free(bloom);
}

int main() {
    test_bloom_create();
    test_bloom_create_invalid_params();
    test_bloom_add_and_check();
    test_bloom_check_nonexistent();
    test_bloom_add_multiple();
    test_bloom_reset();
    test_bloom_stats();
    test_bloom_validate_params();
    test_bloom_serialize_deserialize();
    test_bloom_free_null();
    test_bloom_add_null_data();
    test_bloom_check_null_data();
    test_bloom_stress_many_elements();
    test_bloom_edge_case_single_element();
    test_bloom_edge_case_empty_key();

    return 0;
}
