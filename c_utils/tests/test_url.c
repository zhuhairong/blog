#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/url.h"

void test_url_types() {
    TEST(URL_Types);
    url_config_t config;
    memset(&config, 0, sizeof(config));
    /* url_config_t type compiles and is instantiable */

    url_t url;
    memset(&url, 0, sizeof(url));
    /* url_t type compiles and is instantiable */
}

void test_url_error_values() {
    TEST(URL_ErrorValues);
    EXPECT_TRUE(URL_OK == 0);
    EXPECT_TRUE(URL_INVALID_PARAMS != 0);
    EXPECT_TRUE(URL_INVALID_URL != 0);
}

void test_url_config_fields() {
    TEST(URL_ConfigFields);
    url_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.strict_mode = true;
    config.max_url_length = 2048;
    config.allow_fragment = true;
    
    EXPECT_TRUE(config.strict_mode);
    EXPECT_TRUE(config.max_url_length == 2048);
    EXPECT_TRUE(config.allow_fragment);
}

void test_url_query_param() {
    TEST(URL_QueryParam);
    url_query_param_t param;
    memset(&param, 0, sizeof(param));
    /* url_query_param_t type compiles and is instantiable */
}

void test_url_parse_result() {
    TEST(URL_ParseResult);
    url_parse_result_t result;
    memset(&result, 0, sizeof(result));
    /* url_parse_result_t type compiles and is instantiable */
}

int main() {
    UTEST_BEGIN();
    test_url_types();
    test_url_error_values();
    test_url_config_fields();
    test_url_query_param();
    test_url_parse_result();

    UTEST_END();
}
