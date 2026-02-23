#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/dynamic_lib.h"

void test_dynamic_lib_open_invalid() {
    TEST(DynamicLib_OpenInvalid);
    dynamic_lib_error_t error;
    
    dynamic_lib_t* lib = dynamic_lib_open("/nonexistent/library.so", &error);
    EXPECT_TRUE(lib == NULL);
    EXPECT_TRUE(error != DYNAMIC_LIB_OK);
}

void test_dynamic_lib_open_null() {
    TEST(DynamicLib_OpenNull);
    dynamic_lib_error_t error;
    
    dynamic_lib_t* lib = dynamic_lib_open(NULL, &error);
    EXPECT_TRUE(lib == NULL);
    EXPECT_EQ(error, DYNAMIC_LIB_ERROR_INVALID_PARAM);
}

void test_dynamic_lib_close_null() {
    TEST(DynamicLib_CloseNull);
    dynamic_lib_error_t error;
    
    bool result = dynamic_lib_close(NULL, &error);
    EXPECT_FALSE(result);
}

void test_dynamic_lib_get_symbol_null() {
    TEST(DynamicLib_GetSymbolNull);
    dynamic_lib_error_t error;
    
    void* sym = dynamic_lib_get_symbol(NULL, "test", &error);
    EXPECT_TRUE(sym == NULL);
    EXPECT_EQ(error, DYNAMIC_LIB_ERROR_INVALID_PARAM);
}

void test_dynamic_lib_is_valid() {
    TEST(DynamicLib_IsValid);
    EXPECT_FALSE(dynamic_lib_is_valid(NULL));
}

int main() {
    test_dynamic_lib_open_invalid();
    test_dynamic_lib_open_null();
    test_dynamic_lib_close_null();
    test_dynamic_lib_get_symbol_null();
    test_dynamic_lib_is_valid();

    return 0;
}
