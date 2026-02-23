#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../c_utils/utest.h"
#include "../c_utils/fs_utils.h"

static const char* test_dir = "/tmp/test_fs_utils_dir";
static const char* test_file = "/tmp/test_fs_utils_dir/test_file.txt";

static void setup_test_env() {
    fs_rmdir(test_dir, true, NULL);
    fs_mkdir(test_dir, true, NULL);
}

static void cleanup_test_env() {
    fs_rmdir(test_dir, true, NULL);
}

void test_fs_default_options() {
    TEST(Fs_DefaultOptions);
    fs_options_t opts = fs_default_options();
    EXPECT_TRUE(opts.buffer_size > 0);
}

void test_fs_write_read_all() {
    TEST(Fs_WriteReadAll);
    cleanup_test_env();
    setup_test_env();
    
    const char* data = "Hello, World!";
    fs_error_t error;
    bool result = fs_write_all(test_file, data, strlen(data), NULL, &error);
    EXPECT_TRUE(result);
    
    size_t size;
    char* content = fs_read_all(test_file, &size, &error);
    EXPECT_TRUE(content != NULL);
    EXPECT_EQ(size, strlen(data));
    EXPECT_EQ(memcmp(content, data, size), 0);
    
    free(content);
    cleanup_test_env();
}

void test_fs_exists() {
    TEST(Fs_Exists);
    
    fs_error_t error;
    EXPECT_TRUE(fs_exists("/tmp", &error));
    EXPECT_FALSE(fs_exists("/nonexistent_path_12345", &error));
}

void test_fs_file_size() {
    TEST(Fs_FileSize);
    setup_test_env();
    
    const char* data = "1234567890";
    fs_error_t error;
    fs_write_all(test_file, data, strlen(data), NULL, &error);
    
    long size = fs_file_size(test_file, &error);
    EXPECT_EQ(size, (long)strlen(data));
    
    cleanup_test_env();
}

void test_fs_extname() {
    TEST(Fs_Extname);
    fs_error_t error;
    
    const char* ext = fs_extname("/path/to/file.txt", &error);
    EXPECT_TRUE(ext != NULL);
    if (ext && strlen(ext) > 0) {
        EXPECT_EQ(strcmp(ext, ".txt"), 0);
    }
}

void test_fs_basename() {
    TEST(Fs_Basename);
    fs_error_t error;
    
    const char* base = fs_basename("/path/to/file.txt", &error);
    EXPECT_TRUE(base != NULL);
}

void test_fs_dirname() {
    TEST(Fs_Dirname);
    fs_error_t error;
    
    const char* dir = fs_dirname("/path/to/file.txt", &error);
    EXPECT_TRUE(dir != NULL);
}

void test_fs_mkdir_rmdir() {
    TEST(Fs_MkdirRmdir);
    fs_rmdir("/tmp/test_fs_mkdir_recursive", true, NULL);
    
    const char* new_dir = "/tmp/test_fs_mkdir_recursive/a/b/c";
    
    fs_error_t error;
    bool result = fs_mkdir(new_dir, true, &error);
    EXPECT_TRUE(result);
    if (result) {
        EXPECT_TRUE(fs_exists(new_dir, &error));
        
        result = fs_rmdir("/tmp/test_fs_mkdir_recursive", true, &error);
    }
    fs_rmdir("/tmp/test_fs_mkdir_recursive", true, NULL);
}

void test_fs_unlink() {
    TEST(Fs_Unlink);
    setup_test_env();
    
    fs_error_t error;
    fs_write_all(test_file, "test", 4, NULL, &error);
    EXPECT_TRUE(fs_exists(test_file, &error));
    
    bool result = fs_unlink(test_file, &error);
    EXPECT_TRUE(result);
    EXPECT_FALSE(fs_exists(test_file, &error));
    
    cleanup_test_env();
}

void test_fs_rename() {
    TEST(Fs_Rename);
    setup_test_env();
    
    const char* new_file = "/tmp/test_fs_utils_dir/renamed.txt";
    fs_error_t error;
    fs_write_all(test_file, "test", 4, NULL, &error);
    
    bool result = fs_rename(test_file, new_file, &error);
    EXPECT_TRUE(result);
    EXPECT_FALSE(fs_exists(test_file, &error));
    EXPECT_TRUE(fs_exists(new_file, &error));
    
    fs_unlink(new_file, &error);
    cleanup_test_env();
}

void test_fs_copy() {
    TEST(Fs_Copy);
    setup_test_env();
    
    const char* copy_file = "/tmp/test_fs_utils_dir/copy.txt";
    const char* data = "Hello, Copy!";
    fs_error_t error;
    fs_write_all(test_file, data, strlen(data), NULL, &error);
    
    bool result = fs_copy(test_file, copy_file, NULL, &error);
    EXPECT_TRUE(result);
    EXPECT_TRUE(fs_exists(copy_file, &error));
    
    size_t size;
    char* content = fs_read_all(copy_file, &size, &error);
    EXPECT_TRUE(content != NULL);
    EXPECT_EQ(size, strlen(data));
    
    free(content);
    fs_unlink(copy_file, &error);
    cleanup_test_env();
}

int main() {
    test_fs_default_options();
    test_fs_write_read_all();
    test_fs_exists();
    test_fs_file_size();
    test_fs_extname();
    test_fs_basename();
    test_fs_dirname();
    test_fs_mkdir_rmdir();
    test_fs_unlink();
    test_fs_rename();
    test_fs_copy();

    return 0;
}
