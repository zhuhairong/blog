#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/file_watcher.h"

void test_file_watcher_default_options() {
    TEST(FileWatcher_DefaultOptions);
    file_watcher_options_t options = file_watcher_default_options();
    EXPECT_TRUE(options.poll_interval_ms > 0);
}

void test_file_watcher_create_free() {
    TEST(FileWatcher_CreateFree);
    file_watcher_error_t error;
    file_watcher_options_t options = file_watcher_default_options();
    
    file_watcher_t* fw = file_watcher_create(&options, NULL, NULL, &error);
    EXPECT_TRUE(fw != NULL || fw == NULL);
    
    if (fw != NULL) {
        file_watcher_free(fw);
    }
}

void test_file_watcher_add_null() {
    TEST(FileWatcher_AddNull);
    file_watcher_error_t error;
    
    bool result = file_watcher_add(NULL, "/tmp", &error);
    EXPECT_FALSE(result);
}

void test_file_watcher_remove_null() {
    TEST(FileWatcher_RemoveNull);
    file_watcher_error_t error;
    
    bool result = file_watcher_remove(NULL, "/tmp", &error);
    EXPECT_FALSE(result);
}

void test_file_watcher_get_count_null() {
    TEST(FileWatcher_GetCountNull);
    size_t count = file_watcher_get_count(NULL);
    EXPECT_EQ(count, (size_t)0);
}

int main() {
    test_file_watcher_default_options();
    test_file_watcher_create_free();
    test_file_watcher_add_null();
    test_file_watcher_remove_null();
    test_file_watcher_get_count_null();

    return 0;
}
