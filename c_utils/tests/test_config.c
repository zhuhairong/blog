#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/config.h"

void test_config_create_free() {
    TEST(Config_CreateFree);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_free(cfg);
}

void test_config_get_string() {
    TEST(Config_GetString);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    const char* val = config_get_string(cfg, NULL, "nonexistent", "default");
    EXPECT_TRUE(strcmp(val, "default") == 0);
    
    config_free(cfg);
}

void test_config_get_int() {
    TEST(Config_GetInt);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    int val = config_get_int(cfg, NULL, "nonexistent", 42);
    EXPECT_EQ(val, 42);
    
    config_free(cfg);
}

void test_config_get_double() {
    TEST(Config_GetDouble);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    double val = config_get_double(cfg, NULL, "nonexistent", 3.14);
    EXPECT_TRUE(val > 3.13 && val < 3.15);
    
    config_free(cfg);
}

void test_config_get_bool() {
    TEST(Config_GetBool);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    bool val = config_get_bool(cfg, NULL, "nonexistent", true);
    EXPECT_TRUE(val);
    
    config_free(cfg);
}

int main() {
    test_config_create_free();
    test_config_get_string();
    test_config_get_int();
    test_config_get_double();
    test_config_get_bool();

    return 0;
}
