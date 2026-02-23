#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../c_utils/utest.h"
#include "../c_utils/log_rotate.h"

static const char* test_log_file = "/tmp/test_log_rotate.log";

static void create_test_file(const char* path, size_t size) {
    FILE* f = fopen(path, "w");
    if (f) {
        char buf[1024] = {0};
        memset(buf, 'A', sizeof(buf) - 1);
        size_t written = 0;
        while (written < size) {
            size_t to_write = (size - written) > sizeof(buf) ? sizeof(buf) : (size - written);
            fwrite(buf, 1, to_write, f);
            written += to_write;
        }
        fclose(f);
    }
}

static void cleanup_test_files() {
    unlink(test_log_file);
    char backup[256];
    for (int i = 1; i <= 5; i++) {
        snprintf(backup, sizeof(backup), "%s.%d", test_log_file, i);
        unlink(backup);
    }
}

void test_log_rotate_basic() {
    TEST(LogRotate_Basic);
    cleanup_test_files();
    
    create_test_file(test_log_file, 1024);
    
    log_rotate(test_log_file, 512, 3);
    
    EXPECT_TRUE(access("/tmp/test_log_rotate.log.1", F_OK) == 0);
    
    cleanup_test_files();
}

void test_log_rotate_needs_rotation() {
    TEST(LogRotate_NeedsRotation);
    cleanup_test_files();
    
    log_rotate_config_t config;
    log_rotate_get_default_config(&config);
    config.max_size = 512;
    
    create_test_file(test_log_file, 1024);
    
    log_rotate_error_t error;
    bool needs = log_rotate_needs_rotation(test_log_file, &config, &error);
    EXPECT_TRUE(needs);
    
    cleanup_test_files();
}

void test_log_rotate_no_rotation_needed() {
    TEST(LogRotate_NoRotationNeeded);
    cleanup_test_files();
    
    log_rotate_config_t config;
    log_rotate_get_default_config(&config);
    config.max_size = 2048;
    
    create_test_file(test_log_file, 512);
    
    log_rotate_error_t error;
    bool needs = log_rotate_needs_rotation(test_log_file, &config, &error);
    EXPECT_FALSE(needs);
    
    cleanup_test_files();
}

void test_log_rotate_get_file_size() {
    TEST(LogRotate_GetFileSize);
    cleanup_test_files();
    
    create_test_file(test_log_file, 1024);
    
    size_t size;
    log_rotate_error_t error = log_rotate_get_file_size(test_log_file, &size);
    EXPECT_EQ(error, LOG_ROTATE_OK);
    EXPECT_EQ(size, (size_t)1024);
    
    cleanup_test_files();
}

void test_log_rotate_get_file_size_nonexistent() {
    TEST(LogRotate_GetFileSizeNonexistent);
    
    size_t size;
    log_rotate_error_t error = log_rotate_get_file_size("/nonexistent/file.log", &size);
    EXPECT_TRUE(error != LOG_ROTATE_OK || size == 0);
}

void test_log_rotate_default_config() {
    TEST(LogRotate_DefaultConfig);
    log_rotate_config_t config;
    log_rotate_get_default_config(&config);
    
    EXPECT_TRUE(config.max_size > 0);
    EXPECT_TRUE(config.max_backups > 0);
}

void test_log_rotate_force() {
    TEST(LogRotate_Force);
    cleanup_test_files();
    
    create_test_file(test_log_file, 100);
    
    log_rotate_config_t config;
    log_rotate_get_default_config(&config);
    config.max_backups = 3;
    
    log_rotate_error_t error = log_rotate_force(test_log_file, &config);
    EXPECT_EQ(error, LOG_ROTATE_OK);
    
    EXPECT_TRUE(access("/tmp/test_log_rotate.log.1", F_OK) == 0);
    
    cleanup_test_files();
}

void test_log_rotate_ex() {
    TEST(LogRotate_Ex);
    cleanup_test_files();
    
    create_test_file(test_log_file, 1024);
    
    log_rotate_config_t config;
    log_rotate_get_default_config(&config);
    config.max_size = 512;
    config.max_backups = 3;
    
    bool rotated = false;
    log_rotate_error_t error = log_rotate_ex(test_log_file, &config, LOG_ROTATE_TRIGGER_SIZE, &rotated);
    EXPECT_EQ(error, LOG_ROTATE_OK);
    EXPECT_TRUE(rotated);
    
    cleanup_test_files();
}

void test_log_rotate_cleanup() {
    TEST(LogRotate_Cleanup);
    cleanup_test_files();
    
    create_test_file("/tmp/test_log_rotate.log.1", 100);
    create_test_file("/tmp/test_log_rotate.log.2", 100);
    create_test_file("/tmp/test_log_rotate.log.3", 100);
    create_test_file("/tmp/test_log_rotate.log.4", 100);
    create_test_file("/tmp/test_log_rotate.log.5", 100);
    
    log_rotate_error_t error;
    log_rotate_error_t result = log_rotate_cleanup(test_log_file, 3, &error);
    EXPECT_EQ(result, LOG_ROTATE_OK);
    
    EXPECT_TRUE(access("/tmp/test_log_rotate.log.4", F_OK) != 0);
    EXPECT_TRUE(access("/tmp/test_log_rotate.log.5", F_OK) != 0);
    
    cleanup_test_files();
}

int main() {
    test_log_rotate_basic();
    test_log_rotate_needs_rotation();
    test_log_rotate_no_rotation_needed();
    test_log_rotate_get_file_size();
    test_log_rotate_get_file_size_nonexistent();
    test_log_rotate_default_config();
    test_log_rotate_force();
    test_log_rotate_ex();
    test_log_rotate_cleanup();

    return 0;
}
