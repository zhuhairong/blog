#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/toml_parse.h"

void test_toml_get_string_basic() {
    TEST(Toml_GetStringBasic);
    const char* toml = "name = \"test\"\nvalue = \"hello\"";
    char out[64] = {0};
    
    const char* result = toml_get_string(toml, "name", out, sizeof(out));
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strcmp(out, "test") == 0);
}

void test_toml_get_string_not_found() {
    TEST(Toml_GetStringNotFound);
    const char* toml = "name = \"test\"";
    char out[64] = {0};
    
    const char* result = toml_get_string(toml, "missing", out, sizeof(out));
    EXPECT_TRUE(result == NULL);
}

void test_toml_get_string_multiple() {
    TEST(Toml_GetStringMultiple);
    const char* toml = "key1 = \"value1\"\nkey2 = \"value2\"\nkey3 = \"value3\"";
    char out[64] = {0};
    
    const char* result = toml_get_string(toml, "key2", out, sizeof(out));
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strcmp(out, "value2") == 0);
}

void test_toml_get_string_empty() {
    TEST(Toml_GetStringEmpty);
    const char* toml = "";
    char out[64] = {0};
    
    const char* result = toml_get_string(toml, "key", out, sizeof(out));
    EXPECT_TRUE(result == NULL);
}

void test_toml_get_string_quoted() {
    TEST(Toml_GetStringQuoted);
    const char* toml = "message = \"hello world\"";
    char out[64] = {0};
    
    const char* result = toml_get_string(toml, "message", out, sizeof(out));
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strcmp(out, "hello world") == 0);
}

int main() {
    test_toml_get_string_basic();
    test_toml_get_string_not_found();
    test_toml_get_string_multiple();
    test_toml_get_string_empty();
    test_toml_get_string_quoted();

    return 0;
}
