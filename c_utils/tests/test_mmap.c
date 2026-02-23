#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/mmap.h"

void test_mmap_default_config() {
    TEST(Mmap_DefaultConfig);
    mmap_config_t config = mmap_default_config();
    EXPECT_TRUE(config.read_only);
    EXPECT_TRUE(config.private);
    EXPECT_FALSE(config.populate);
    EXPECT_EQ(config.offset, (size_t)0);
    EXPECT_EQ(config.length, (size_t)0);
}

void test_mmap_file_and_free() {
    TEST(Mmap_FileAndFree);
    const char* test_file = "test_mmap_file.txt";
    FILE* f = fopen(test_file, "wb");
    const char* data = "Hello World from mmap!";
    fwrite(data, 1, strlen(data), f);
    fclose(f);
    
    size_t size;
    char* ptr = (char*)mmap_file(test_file, &size);
    EXPECT_TRUE(ptr != NULL);
    EXPECT_TRUE(size > 0);
    EXPECT_STREQ(ptr, data);
    
    mmap_free(ptr, size);
    remove(test_file);
}

void test_mmap_file_null() {
    TEST(Mmap_FileNull);
    size_t size;
    void* ptr = mmap_file(NULL, &size);
    EXPECT_TRUE(ptr == NULL);
}

void test_mmap_file_ex() {
    TEST(Mmap_FileEx);
    const char* test_file = "test_mmap_file_ex.txt";
    FILE* f = fopen(test_file, "wb");
    const char* data = "Hello World from mmap ex!";
    fwrite(data, 1, strlen(data), f);
    fclose(f);
    
    size_t size;
    mmap_config_t config = mmap_default_config();
    mmap_error_t error;
    char* ptr = (char*)mmap_file_ex(test_file, &size, &config, &error);
    EXPECT_TRUE(ptr != NULL);
    EXPECT_EQ(error, MMAP_OK);
    EXPECT_STREQ(ptr, data);
    
    mmap_free(ptr, size);
    remove(test_file);
}

void test_mmap_file_ex_nonexistent() {
    TEST(Mmap_FileExNonexistent);
    size_t size;
    mmap_error_t error;
    void* ptr = mmap_file_ex("nonexistent_file.txt", &size, NULL, &error);
    EXPECT_TRUE(ptr == NULL);
    EXPECT_EQ(error, MMAP_ERROR_OPEN_FILE);
}

void test_mmap_file_info() {
    TEST(Mmap_FileInfo);
    const char* test_file = "test_mmap_file_info.txt";
    FILE* f = fopen(test_file, "wb");
    const char* data = "Hello World from mmap info!";
    fwrite(data, 1, strlen(data), f);
    fclose(f);
    
    mmap_config_t config = mmap_default_config();
    mmap_info_t info;
    mmap_error_t error;
    bool success = mmap_file_info(test_file, &config, &info, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, MMAP_OK);
    EXPECT_TRUE(info.addr != NULL);
    EXPECT_TRUE(info.size > 0);
    EXPECT_TRUE(info.read_only);
    EXPECT_TRUE(info.private);
    
    mmap_free(info.addr, info.size);
    remove(test_file);
}

void test_mmap_file_info_null() {
    TEST(Mmap_FileInfoNull);
    mmap_error_t error;
    bool success = mmap_file_info(NULL, NULL, NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, MMAP_ERROR_NULL_PTR);
}

void test_mmap_free_ex() {
    TEST(Mmap_FreeEx);
    void* ptr = mmap_anonymous(4096, false, NULL);
    mmap_error_t error;
    bool success = mmap_free_ex(ptr, 4096, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, MMAP_OK);
}

void test_mmap_free_ex_null() {
    TEST(Mmap_FreeExNull);
    mmap_error_t error;
    bool success = mmap_free_ex(NULL, 4096, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, MMAP_ERROR_NULL_PTR);
}

void test_mmap_anonymous() {
    TEST(Mmap_Anonymous);
    mmap_error_t error;
    char* ptr = (char*)mmap_anonymous(4096, false, &error);
    EXPECT_TRUE(ptr != NULL);
    EXPECT_EQ(error, MMAP_OK);
    
    strcpy(ptr, "Test anonymous mmap");
    EXPECT_STREQ(ptr, "Test anonymous mmap");
    
    mmap_free(ptr, 4096);
}

void test_mmap_anonymous_zero_size() {
    TEST(Mmap_AnonymousZeroSize);
    mmap_error_t error;
    void* ptr = mmap_anonymous(0, false, &error);
    EXPECT_TRUE(ptr == NULL);
    EXPECT_EQ(error, MMAP_ERROR_INVALID_ARGS);
}

void test_mmap_error_string() {
    TEST(Mmap_ErrorString);
    const char* msg = mmap_error_string(MMAP_OK);
    EXPECT_STREQ(msg, "Success");
    msg = mmap_error_string(MMAP_ERROR_NULL_PTR);
    EXPECT_STREQ(msg, "Null pointer error");
    msg = mmap_error_string(9999);
    EXPECT_STREQ(msg, "Unknown error");
}

int main() {
    test_mmap_default_config();
    test_mmap_file_and_free();
    test_mmap_file_null();
    test_mmap_file_ex();
    test_mmap_file_ex_nonexistent();
    test_mmap_file_info();
    test_mmap_file_info_null();
    test_mmap_free_ex();
    test_mmap_free_ex_null();
    test_mmap_anonymous();
    test_mmap_anonymous_zero_size();
    test_mmap_error_string();

    return 0;
}
