#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/coap_pkt.h"

void test_coap_types() {
    TEST(CoAP_Types);
    EXPECT_EQ(COAP_TYPE_CON, 0);
    EXPECT_EQ(COAP_TYPE_NON, 1);
    EXPECT_EQ(COAP_TYPE_ACK, 2);
    EXPECT_EQ(COAP_TYPE_RST, 3);
}

void test_coap_methods() {
    TEST(CoAP_Methods);
    EXPECT_EQ(COAP_METHOD_GET, 1);
    EXPECT_EQ(COAP_METHOD_POST, 2);
    EXPECT_EQ(COAP_METHOD_PUT, 3);
    EXPECT_EQ(COAP_METHOD_DELETE, 4);
}

void test_coap_response_codes() {
    TEST(CoAP_ResponseCodes);
    EXPECT_EQ(COAP_CODE_200_OK, 65);
    EXPECT_EQ(COAP_CODE_404_NOT_FOUND, 132);
    EXPECT_EQ(COAP_CODE_500_INTERNAL_SERVER_ERROR, 160);
}

void test_coap_options() {
    TEST(CoAP_Options);
    EXPECT_EQ(COAP_OPTION_URI_HOST, 3);
    EXPECT_EQ(COAP_OPTION_URI_PATH, 11);
    EXPECT_EQ(COAP_OPTION_CONTENT_FORMAT, 12);
}

void test_coap_error_values() {
    TEST(CoAP_ErrorValues);
    EXPECT_TRUE(COAP_OK == 0);
    EXPECT_TRUE(COAP_ERROR_INVALID_HEADER != 0);
    EXPECT_TRUE(COAP_ERROR_INVALID_PARAM != 0);
}

int main() {
    test_coap_types();
    test_coap_methods();
    test_coap_response_codes();
    test_coap_options();
    test_coap_error_values();

    return 0;
}
