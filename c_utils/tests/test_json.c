#include "../c_utils/utest.h"
#include "../c_utils/json.h"
#include <string.h>

void test_json_parse_null() {
    TEST(JSON_ParseNull);
    json_value_t* v = json_parse("null");
    EXPECT_TRUE(v != NULL);
    EXPECT_EQ(json_type(v), JSON_NULL);
    json_free(v);
}

void test_json_parse_bool() {
    TEST(JSON_ParseBool);
    json_value_t* t = json_parse("true");
    EXPECT_TRUE(t != NULL);
    EXPECT_EQ(json_type(t), JSON_BOOL);
    EXPECT_TRUE(json_as_bool(t));
    json_free(t);
    
    json_value_t* f = json_parse("false");
    EXPECT_TRUE(f != NULL);
    EXPECT_EQ(json_type(f), JSON_BOOL);
    EXPECT_TRUE(!json_as_bool(f));
    json_free(f);
}

void test_json_parse_number() {
    TEST(JSON_ParseNumber);
    json_value_t* v = json_parse("42.5");
    EXPECT_TRUE(v != NULL);
    EXPECT_EQ(json_type(v), JSON_NUMBER);
    EXPECT_TRUE(json_as_number(v) > 42.4 && json_as_number(v) < 42.6);
    json_free(v);
}

void test_json_parse_string() {
    TEST(JSON_ParseString);
    json_value_t* v = json_parse("\"hello\"");
    EXPECT_TRUE(v != NULL);
    EXPECT_EQ(json_type(v), JSON_STRING);
    EXPECT_STR_EQ(json_as_string(v), "hello");
    json_free(v);
}

void test_json_parse_array() {
    TEST(JSON_ParseArray);
    json_value_t* v = json_parse("[1, 2, 3]");
    EXPECT_TRUE(v != NULL);
    EXPECT_EQ(json_type(v), JSON_ARRAY);
    EXPECT_EQ(json_array_size(v), 3);
    json_free(v);
}

void test_json_parse_object() {
    TEST(JSON_ParseObject);
    json_value_t* v = json_parse("{\"name\": \"test\", \"value\": 42}");
    EXPECT_TRUE(v != NULL);
    EXPECT_EQ(json_type(v), JSON_OBJECT);
    
    json_value_t* name = json_object_get(v, "name");
    EXPECT_TRUE(name != NULL);
    EXPECT_STR_EQ(json_as_string(name), "test");
    
    json_free(v);
}

void test_json_parse_nested() {
    TEST(JSON_ParseNested);
    const char* json = "{\"outer\": {\"inner\": [1, 2, 3]}}";
    json_value_t* v = json_parse(json);
    EXPECT_TRUE(v != NULL);
    
    json_value_t* outer = json_object_get(v, "outer");
    EXPECT_TRUE(outer != NULL);
    
    json_value_t* inner = json_object_get(outer, "inner");
    EXPECT_TRUE(inner != NULL);
    EXPECT_EQ(json_type(inner), JSON_ARRAY);
    EXPECT_EQ(json_array_size(inner), 3);
    
    json_free(v);
}

void test_json_serialize() {
    TEST(JSON_Serialize);
    json_value_t* v = json_parse("{\"a\":1}");
    EXPECT_TRUE(v != NULL);
    
    char* str = json_print(v);
    EXPECT_TRUE(str != NULL);
    EXPECT_TRUE(strlen(str) > 0);
    free(str);
    json_free(v);
}

int main() {
    UTEST_BEGIN();
    test_json_parse_null();
    test_json_parse_bool();
    test_json_parse_number();
    test_json_parse_string();
    test_json_parse_array();
    test_json_parse_object();
    test_json_parse_nested();
    test_json_serialize();
    UTEST_END();
}
