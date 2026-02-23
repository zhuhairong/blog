#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/dns_pkt.h"

void test_dns_types() {
    TEST(DNS_Types);
    dns_header_t header;
    memset(&header, 0, sizeof(header));
    EXPECT_TRUE(sizeof(header) > 0);
    
    dns_question_t question;
    memset(&question, 0, sizeof(question));
    EXPECT_TRUE(sizeof(question) > 0);
}

void test_dns_error_values() {
    TEST(DNS_ErrorValues);
    EXPECT_TRUE(DNS_OK == 0);
    EXPECT_TRUE(DNS_ERROR_INVALID_PARAM != 0);
    EXPECT_TRUE(DNS_ERROR_INVALID_PACKET != 0);
}

void test_dns_record_types() {
    TEST(DNS_RecordTypes);
    EXPECT_EQ(DNS_TYPE_A, 1);
    EXPECT_EQ(DNS_TYPE_NS, 2);
    EXPECT_EQ(DNS_TYPE_CNAME, 5);
    EXPECT_EQ(DNS_TYPE_MX, 15);
    EXPECT_EQ(DNS_TYPE_AAAA, 28);
}

void test_dns_class_values() {
    TEST(DNS_ClassValues);
    EXPECT_EQ(DNS_CLASS_IN, 1);
    EXPECT_EQ(DNS_CLASS_ANY, 255);
}

void test_dns_header_size() {
    TEST(DNS_HeaderSize);
    dns_header_t header;
    EXPECT_TRUE(sizeof(header) >= 12);
}

int main() {
    test_dns_types();
    test_dns_error_values();
    test_dns_record_types();
    test_dns_class_values();
    test_dns_header_size();

    return 0;
}
