#include "../c_utils/utest.h"
#include "../c_utils/env.h"
#include <string.h>

void test_env_basic() {
    TEST(Env_SetGet);
    env_error_t err;
    bool result = env_set("TEST_VAR", "test_value", true, NULL, &err);
    EXPECT_TRUE(result);
    
    const char* value = env_get("TEST_VAR", "default", NULL, &err);
    EXPECT_TRUE(value != NULL);
    EXPECT_STR_EQ(value, "test_value");
}

void test_env_default() {
    TEST(Env_Default);
    env_error_t err;
    const char* value = env_get("NON_EXISTENT_VAR_12345", "default_val", NULL, &err);
    EXPECT_STR_EQ(value, "default_val");
}

void test_env_has() {
    TEST(Env_Has);
    env_error_t err;
    env_set("TEST_VAR_HAS", "value", true, NULL, &err);
    EXPECT_TRUE(env_has("TEST_VAR_HAS", NULL, &err));
    EXPECT_TRUE(!env_has("NON_EXISTENT_VAR_HAS", NULL, &err));
    env_unset("TEST_VAR_HAS", NULL, &err);
}

void test_env_unset() {
    TEST(Env_Unset);
    env_error_t err;
    env_set("TEST_VAR_UNSET", "value", true, NULL, &err);
    EXPECT_TRUE(env_has("TEST_VAR_UNSET", NULL, &err));
    
    bool result = env_unset("TEST_VAR_UNSET", NULL, &err);
    EXPECT_TRUE(result);
    EXPECT_TRUE(!env_has("TEST_VAR_UNSET", NULL, &err));
}

void test_env_int() {
    TEST(Env_Int);
    env_error_t err;
    env_set("TEST_INT_VAR", "42", true, NULL, &err);
    int value = env_get_int("TEST_INT_VAR", 0, NULL, &err);
    EXPECT_EQ(value, 42);
    
    value = env_get_int("NON_EXISTENT_INT", 100, NULL, &err);
    EXPECT_EQ(value, 100);
    
    env_unset("TEST_INT_VAR", NULL, &err);
}

void test_env_bool() {
    TEST(Env_Bool);
    env_error_t err;
    env_set("TEST_BOOL_VAR", "true", true, NULL, &err);
    bool value = env_get_bool("TEST_BOOL_VAR", false, NULL, &err);
    EXPECT_TRUE(value);
    
    value = env_get_bool("NON_EXISTENT_BOOL", true, NULL, &err);
    EXPECT_TRUE(value);
    
    env_unset("TEST_BOOL_VAR", NULL, &err);
}

int main() {
    UTEST_BEGIN();
    test_env_basic();
    test_env_default();
    test_env_has();
    test_env_unset();
    test_env_int();
    test_env_bool();
    UTEST_END();
}
