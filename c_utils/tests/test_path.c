#include "../c_utils/utest.h"
#include "../c_utils/path.h"
#include <string.h>
#include <stdlib.h>

void test_path_join() {
    TEST(Path_Join);
    char* result = path_join("/home", "user");
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strstr(result, "home") != NULL);
    EXPECT_TRUE(strstr(result, "user") != NULL);
    free(result);
}

void test_path_join_ex() {
    TEST(Path_JoinEx);
    path_error_t err;
    path_config_t config = path_default_config();
    char* result = path_join_ex("/home", "user", &config, &err);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(err, PATH_OK);
    free(result);
}

void test_path_dirname() {
    TEST(Path_Dirname);
    char* result = path_dirname("/home/user/file.txt");
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strstr(result, "home") != NULL || strstr(result, "user") != NULL);
    free(result);
}

void test_path_basename() {
    TEST(Path_Basename);
    const char* base = path_basename("/home/user/file.txt");
    EXPECT_TRUE(base != NULL);
    EXPECT_STR_EQ(base, "file.txt");
}

void test_path_extension() {
    TEST(Path_Extension);
    const char* ext = path_get_extension("/home/user/file.txt");
    EXPECT_TRUE(ext != NULL);
    EXPECT_STR_EQ(ext, ".txt");
    
    ext = path_get_extension("/home/user/file");
    EXPECT_TRUE(ext != NULL);
    EXPECT_STR_EQ(ext, "");
}

void test_path_is_absolute() {
    TEST(Path_IsAbsolute);
    EXPECT_TRUE(path_is_absolute("/home/user"));
    EXPECT_TRUE(!path_is_absolute("relative/path"));
}

void test_path_is_relative() {
    TEST(Path_IsRelative);
    EXPECT_TRUE(path_is_relative("relative/path"));
    EXPECT_TRUE(!path_is_relative("/home/user"));
}

void test_path_normalize() {
    TEST(Path_Normalize);
    path_error_t err;
    char* result = path_normalize("/home/../user/./file.txt", &err);
    EXPECT_TRUE(result != NULL);
    free(result);
}

int main() {
    UTEST_BEGIN();
    test_path_join();
    test_path_join_ex();
    test_path_dirname();
    test_path_basename();
    test_path_extension();
    test_path_is_absolute();
    test_path_is_relative();
    test_path_normalize();
    UTEST_END();
}
