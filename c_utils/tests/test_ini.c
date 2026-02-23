#include "../c_utils/utest.h"
#include "../c_utils/ini.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static const char* test_ini_file = "/tmp/test_ini.ini";

static void create_test_ini() {
    FILE* f = fopen(test_ini_file, "w");
    if (f) {
        fprintf(f, "[section1]\n");
        fprintf(f, "key1 = value1\n");
        fprintf(f, "key2 = 42\n");
        fprintf(f, "key3 = true\n");
        fprintf(f, "\n");
        fprintf(f, "[section2]\n");
        fprintf(f, "name = test\n");
        fclose(f);
    }
}

static void cleanup_test_ini() {
    unlink(test_ini_file);
}

void test_ini_load() {
    TEST(INI_Load);
    create_test_ini();
    
    ini_t* ini = ini_load(test_ini_file);
    EXPECT_TRUE(ini != NULL);
    ini_free(ini);
    
    cleanup_test_ini();
}

void test_ini_get_string() {
    TEST(INI_GetString);
    create_test_ini();
    
    ini_t* ini = ini_load(test_ini_file);
    EXPECT_TRUE(ini != NULL);
    
    const char* val = ini_get(ini, "section1", "key1");
    EXPECT_TRUE(val != NULL);
    EXPECT_STR_EQ(val, "value1");
    
    ini_free(ini);
    cleanup_test_ini();
}

void test_ini_get_int() {
    TEST(INI_GetInt);
    create_test_ini();
    
    ini_t* ini = ini_load(test_ini_file);
    EXPECT_TRUE(ini != NULL);
    
    int val = ini_get_int(ini, "section1", "key2", 0);
    EXPECT_EQ(val, 42);
    
    ini_free(ini);
    cleanup_test_ini();
}

void test_ini_get_bool() {
    TEST(INI_GetBool);
    create_test_ini();
    
    ini_t* ini = ini_load(test_ini_file);
    EXPECT_TRUE(ini != NULL);
    
    bool val = ini_get_bool(ini, "section1", "key3", false);
    EXPECT_TRUE(val);
    
    ini_free(ini);
    cleanup_test_ini();
}

void test_ini_set() {
    TEST(INI_Set);
    ini_t* ini = ini_create();
    EXPECT_TRUE(ini != NULL);
    
    EXPECT_TRUE(ini_set(ini, "section", "key", "value"));
    
    const char* val = ini_get(ini, "section", "key");
    EXPECT_TRUE(val != NULL);
    EXPECT_STR_EQ(val, "value");
    
    ini_free(ini);
}

void test_ini_has_section() {
    TEST(INI_HasSection);
    create_test_ini();
    
    ini_t* ini = ini_load(test_ini_file);
    EXPECT_TRUE(ini != NULL);
    
    EXPECT_TRUE(ini_has_section(ini, "section1"));
    EXPECT_TRUE(!ini_has_section(ini, "nonexistent"));
    
    ini_free(ini);
    cleanup_test_ini();
}

void test_ini_has_key() {
    TEST(INI_HasKey);
    create_test_ini();
    
    ini_t* ini = ini_load(test_ini_file);
    EXPECT_TRUE(ini != NULL);
    
    EXPECT_TRUE(ini_has_key(ini, "section1", "key1"));
    EXPECT_TRUE(!ini_has_key(ini, "section1", "nonexistent"));
    
    ini_free(ini);
    cleanup_test_ini();
}

void test_ini_save() {
    TEST(INI_Save);
    ini_t* ini = ini_create();
    EXPECT_TRUE(ini != NULL);
    
    ini_set(ini, "section", "key", "value");
    
    EXPECT_TRUE(ini_save(ini, test_ini_file));
    
    ini_free(ini);
    
    ini_t* loaded = ini_load(test_ini_file);
    EXPECT_TRUE(loaded != NULL);
    
    const char* val = ini_get(loaded, "section", "key");
    EXPECT_TRUE(val != NULL);
    EXPECT_STR_EQ(val, "value");
    
    ini_free(loaded);
    cleanup_test_ini();
}

int main() {
    UTEST_BEGIN();
    test_ini_load();
    test_ini_get_string();
    test_ini_get_int();
    test_ini_get_bool();
    test_ini_set();
    test_ini_has_section();
    test_ini_has_key();
    test_ini_save();
    UTEST_END();
}
