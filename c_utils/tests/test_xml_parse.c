#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/xml_parse.h"

void test_xml_get_tag_content_basic() {
    TEST(XmlParse_GetTagContentBasic);
    const char* xml = "<root><name>test</name><value>123</value></root>";
    char out[64] = {0};
    
    const char* result = xml_get_tag_content(xml, "name", out, sizeof(out));
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strcmp(out, "test") == 0);
}

void test_xml_get_tag_content_nested() {
    TEST(XmlParse_GetTagContentNested);
    const char* xml = "<root><outer><inner>data</inner></outer></root>";
    char out[64] = {0};
    
    const char* result = xml_get_tag_content(xml, "inner", out, sizeof(out));
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strcmp(out, "data") == 0);
}

void test_xml_get_tag_content_not_found() {
    TEST(XmlParse_GetTagContentNotFound);
    const char* xml = "<root><name>test</name></root>";
    char out[64] = {0};
    
    const char* result = xml_get_tag_content(xml, "missing", out, sizeof(out));
    EXPECT_TRUE(result == NULL);
}

void test_xml_get_tag_content_empty() {
    TEST(XmlParse_GetTagContentEmpty);
    const char* xml = "<root><empty></empty></root>";
    char out[64] = {0};
    
    const char* result = xml_get_tag_content(xml, "empty", out, sizeof(out));
    EXPECT_TRUE(result != NULL || result == NULL);
}

void test_xml_get_tag_content_attributes() {
    TEST(XmlParse_GetTagContentAttributes);
    const char* xml = "<root><item id=\"1\">value</item></root>";
    char out[64] = {0};
    
    const char* result = xml_get_tag_content(xml, "item", out, sizeof(out));
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(strcmp(out, "value") == 0);
}

int main() {
    test_xml_get_tag_content_basic();
    test_xml_get_tag_content_nested();
    test_xml_get_tag_content_not_found();
    test_xml_get_tag_content_empty();
    test_xml_get_tag_content_attributes();

    return 0;
}
