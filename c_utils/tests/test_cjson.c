#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "cjson_wrapper.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("  测试: %s... ", #name); \
    test_##name(); \
    printf("通过\n"); \
    tests_passed++; \
} while(0)

#define ASSERT_TRUE(cond) do { \
    if (!(cond)) { \
        printf("失败: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        printf("失败: %s:%d: %s != %s\n", __FILE__, __LINE__, #a, #b); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_EQ_STR(a, b) do { \
    if (strcmp((a), (b)) != 0) { \
        printf("失败: %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, (a), (b)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NEAR(a, b, eps) do { \
    if (fabs((a) - (b)) > (eps)) { \
        printf("失败: %s:%d: %g != %g\n", __FILE__, __LINE__, (double)(a), (double)(b)); \
        tests_failed++; \
        return; \
    } \
} while(0)

TEST(error_string) {
    const char* str = cjson_error_string(CJSON_WRAP_OK);
    ASSERT_TRUE(str != NULL);
    ASSERT_TRUE(strlen(str) > 0);
    
    str = cjson_error_string(CJSON_WRAP_ERR_NULL_PTR);
    ASSERT_TRUE(str != NULL);
    
    str = cjson_error_string(CJSON_WRAP_ERR_PARSE);
    ASSERT_TRUE(str != NULL);
}

TEST(parse_valid_json) {
    const char* json = "{\"name\": \"test\", \"value\": 42}";
    cjson_value_t* root = cjson_parse(json);
    ASSERT_TRUE(root != NULL);
    
    ASSERT_TRUE(cjson_is_object(root));
    
    const char* name = cjson_object_get_string(root, "name", "");
    ASSERT_EQ_STR(name, "test");
    
    double value = cjson_object_get_number(root, "value", 0);
    ASSERT_EQ(value, 42);
    
    cjson_free(root);
}

TEST(parse_invalid_json) {
    const char* json = "{invalid json}";
    cjson_value_t* root = cjson_parse(json);
    ASSERT_TRUE(root == NULL);
    
    const char* error = cjson_get_error();
    ASSERT_TRUE(error != NULL);
}

TEST(parse_null_input) {
    cjson_value_t* root = cjson_parse(NULL);
    ASSERT_TRUE(root == NULL);
}

TEST(parse_with_length) {
    const char* json = "{\"key\": \"value\"}extra data";
    cjson_value_t* root = cjson_parse_with_length(json, 17);
    ASSERT_TRUE(root != NULL);
    
    const char* key = cjson_object_get_string(root, "key", "");
    ASSERT_EQ_STR(key, "value");
    
    cjson_free(root);
}

TEST(create_null) {
    cjson_value_t* value = cjson_create_null();
    ASSERT_TRUE(value != NULL);
    ASSERT_TRUE(cjson_is_null(value));
    ASSERT_EQ(cjson_get_type(value), CJSON_WRAP_TYPE_NULL);
    
    cjson_free(value);
}

TEST(create_bool) {
    cjson_value_t* value = cjson_create_bool(true);
    ASSERT_TRUE(value != NULL);
    ASSERT_TRUE(cjson_is_bool(value));
    ASSERT_TRUE(cjson_get_bool(value));
    ASSERT_EQ(cjson_get_type(value), CJSON_WRAP_TYPE_BOOL);
    
    cjson_free(value);
    
    value = cjson_create_bool(false);
    ASSERT_TRUE(value != NULL);
    ASSERT_TRUE(!cjson_get_bool(value));
    
    cjson_free(value);
}

TEST(create_number) {
    cjson_value_t* value = cjson_create_number(3.14159);
    ASSERT_TRUE(value != NULL);
    ASSERT_TRUE(cjson_is_number(value));
    ASSERT_NEAR(cjson_get_number(value), 3.14159, 0.00001);
    ASSERT_EQ(cjson_get_type(value), CJSON_WRAP_TYPE_NUMBER);
    
    cjson_free(value);
    
    value = cjson_create_number(-100);
    ASSERT_TRUE(value != NULL);
    ASSERT_NEAR(cjson_get_number(value), -100, 0.001);
    
    cjson_free(value);
}

TEST(create_string) {
    cjson_value_t* value = cjson_create_string("hello world");
    ASSERT_TRUE(value != NULL);
    ASSERT_TRUE(cjson_is_string(value));
    ASSERT_EQ_STR(cjson_get_string(value), "hello world");
    ASSERT_EQ(cjson_get_type(value), CJSON_WRAP_TYPE_STRING);
    
    cjson_free(value);
}

TEST(create_array) {
    cjson_value_t* value = cjson_create_array();
    ASSERT_TRUE(value != NULL);
    ASSERT_TRUE(cjson_is_array(value));
    ASSERT_EQ(cjson_get_type(value), CJSON_WRAP_TYPE_ARRAY);
    ASSERT_EQ(cjson_array_size(value), 0);
    
    cjson_free(value);
}

TEST(create_object) {
    cjson_value_t* value = cjson_create_object();
    ASSERT_TRUE(value != NULL);
    ASSERT_TRUE(cjson_is_object(value));
    ASSERT_EQ(cjson_get_type(value), CJSON_WRAP_TYPE_OBJECT);
    ASSERT_EQ(cjson_object_size(value), 0);
    
    cjson_free(value);
}

TEST(array_operations) {
    cjson_value_t* arr = cjson_create_array();
    ASSERT_TRUE(arr != NULL);
    
    ASSERT_EQ(cjson_array_add(arr, cjson_create_number(1)), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_array_add(arr, cjson_create_number(2)), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_array_add(arr, cjson_create_number(3)), CJSON_WRAP_OK);
    
    ASSERT_EQ(cjson_array_size(arr), 3);
    
    cjson_value_t* item = cjson_array_get(arr, 0);
    ASSERT_TRUE(item != NULL);
    ASSERT_NEAR(cjson_get_number(item), 1, 0.001);
    
    item = cjson_array_get(arr, 2);
    ASSERT_TRUE(item != NULL);
    ASSERT_NEAR(cjson_get_number(item), 3, 0.001);
    
    ASSERT_EQ(cjson_array_set(arr, 1, cjson_create_number(20)), CJSON_WRAP_OK);
    item = cjson_array_get(arr, 1);
    ASSERT_NEAR(cjson_get_number(item), 20, 0.001);
    
    ASSERT_EQ(cjson_array_remove(arr, 0), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_array_size(arr), 2);
    
    item = cjson_array_get(arr, 0);
    ASSERT_NEAR(cjson_get_number(item), 20, 0.001);
    
    cjson_free(arr);
}

TEST(object_operations) {
    cjson_value_t* obj = cjson_create_object();
    ASSERT_TRUE(obj != NULL);
    
    ASSERT_EQ(cjson_object_set_string(obj, "name", "test"), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_object_set_number(obj, "value", 42), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_object_set_bool(obj, "active", true), CJSON_WRAP_OK);
    
    ASSERT_EQ(cjson_object_size(obj), 3);
    
    ASSERT_TRUE(cjson_object_has(obj, "name"));
    ASSERT_TRUE(cjson_object_has(obj, "value"));
    ASSERT_TRUE(!cjson_object_has(obj, "missing"));
    
    ASSERT_EQ_STR(cjson_object_get_string(obj, "name", ""), "test");
    ASSERT_NEAR(cjson_object_get_number(obj, "value", 0), 42, 0.001);
    ASSERT_TRUE(cjson_object_get_bool(obj, "active", false));
    
    ASSERT_EQ(cjson_object_remove(obj, "value"), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_object_size(obj), 2);
    ASSERT_TRUE(!cjson_object_has(obj, "value"));
    
    cjson_free(obj);
}

TEST(object_convenience_functions) {
    cjson_value_t* obj = cjson_create_object();
    ASSERT_TRUE(obj != NULL);
    
    ASSERT_EQ(cjson_object_set_null(obj, "null_key"), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_object_set_bool(obj, "bool_key", true), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_object_set_number(obj, "number_key", 123.45), CJSON_WRAP_OK);
    ASSERT_EQ(cjson_object_set_string(obj, "string_key", "hello"), CJSON_WRAP_OK);
    
    cjson_value_t* nested_obj = cjson_create_object();
    cjson_object_set_string(nested_obj, "nested", "value");
    ASSERT_EQ(cjson_object_set(obj, "object_key", nested_obj), CJSON_WRAP_OK);
    
    cjson_value_t* nested_arr = cjson_create_array();
    cjson_array_add(nested_arr, cjson_create_number(1));
    cjson_array_add(nested_arr, cjson_create_number(2));
    ASSERT_EQ(cjson_object_set(obj, "array_key", nested_arr), CJSON_WRAP_OK);
    
    ASSERT_TRUE(cjson_is_null(cjson_object_get(obj, "null_key")));
    ASSERT_TRUE(cjson_object_get_bool(obj, "bool_key", false));
    ASSERT_NEAR(cjson_object_get_number(obj, "number_key", 0), 123.45, 0.001);
    ASSERT_EQ_STR(cjson_object_get_string(obj, "string_key", ""), "hello");
    
    cjson_value_t* retrieved_obj = cjson_object_get_object(obj, "object_key");
    ASSERT_TRUE(retrieved_obj != NULL);
    ASSERT_TRUE(cjson_is_object(retrieved_obj));
    
    cjson_value_t* retrieved_arr = cjson_object_get_array(obj, "array_key");
    ASSERT_TRUE(retrieved_arr != NULL);
    ASSERT_TRUE(cjson_is_array(retrieved_arr));
    ASSERT_EQ(cjson_array_size(retrieved_arr), 2);
    
    cjson_free(obj);
}

TEST(print_json) {
    cjson_value_t* obj = cjson_create_object();
    cjson_object_set_string(obj, "name", "test");
    cjson_object_set_number(obj, "value", 42);
    
    char* formatted = cjson_print(obj);
    ASSERT_TRUE(formatted != NULL);
    ASSERT_TRUE(strstr(formatted, "name") != NULL);
    ASSERT_TRUE(strstr(formatted, "test") != NULL);
    free(formatted);
    
    char* unformatted = cjson_print_unformatted(obj);
    ASSERT_TRUE(unformatted != NULL);
    ASSERT_TRUE(strstr(unformatted, "name") != NULL);
    ASSERT_TRUE(strstr(unformatted, "test") != NULL);
    free(unformatted);
    
    cjson_free(obj);
}

TEST(file_operations) {
    cjson_value_t* obj = cjson_create_object();
    cjson_object_set_string(obj, "key", "value");
    cjson_object_set_number(obj, "number", 123);
    
    const char* filename = "/tmp/test_cjson.json";
    int result = cjson_print_to_file(obj, filename);
    ASSERT_EQ(result, CJSON_WRAP_OK);
    
    cjson_value_t* loaded = cjson_parse_file(filename);
    ASSERT_TRUE(loaded != NULL);
    
    ASSERT_EQ_STR(cjson_object_get_string(loaded, "key", ""), "value");
    ASSERT_NEAR(cjson_object_get_number(loaded, "number", 0), 123, 0.001);
    
    cjson_free(loaded);
    cjson_free(obj);
}

TEST(deep_copy) {
    cjson_value_t* obj1 = cjson_create_object();
    cjson_object_set_string(obj1, "name", "original");
    cjson_object_set_number(obj1, "value", 100);
    
    cjson_value_t* obj2 = cjson_deep_copy(obj1);
    ASSERT_TRUE(obj2 != NULL);
    
    ASSERT_TRUE(cjson_equal(obj1, obj2));
    
    cjson_object_set_number(obj2, "value", 200);
    ASSERT_TRUE(!cjson_equal(obj1, obj2));
    
    ASSERT_NEAR(cjson_object_get_number(obj1, "value", 0), 100, 0.001);
    ASSERT_NEAR(cjson_object_get_number(obj2, "value", 0), 200, 0.001);
    
    cjson_free(obj1);
    cjson_free(obj2);
}

TEST(equal) {
    cjson_value_t* obj1 = cjson_create_object();
    cjson_object_set_string(obj1, "key", "value");
    
    cjson_value_t* obj2 = cjson_create_object();
    cjson_object_set_string(obj2, "key", "value");
    
    ASSERT_TRUE(cjson_equal(obj1, obj2));
    
    cjson_value_t* obj3 = cjson_create_object();
    cjson_object_set_string(obj3, "key", "different");
    
    ASSERT_TRUE(!cjson_equal(obj1, obj3));
    
    ASSERT_TRUE(cjson_equal(obj1, obj1));
    ASSERT_TRUE(!cjson_equal(obj1, NULL));
    ASSERT_TRUE(!cjson_equal(NULL, obj1));
    ASSERT_TRUE(cjson_equal(NULL, NULL));
    
    cjson_free(obj1);
    cjson_free(obj2);
    cjson_free(obj3);
}

TEST(nested_structures) {
    const char* json = "{"
        "\"user\": {"
        "  \"name\": \"张三\","
        "  \"age\": 25,"
        "  \"contacts\": ["
        "    {\"type\": \"email\", \"value\": \"test@example.com\"},"
        "    {\"type\": \"phone\", \"value\": \"1234567890\"}"
        "  ]"
        "}"
    "}";
    
    cjson_value_t* root = cjson_parse(json);
    ASSERT_TRUE(root != NULL);
    
    cjson_value_t* user = cjson_object_get_object(root, "user");
    ASSERT_TRUE(user != NULL);
    
    ASSERT_EQ_STR(cjson_object_get_string(user, "name", ""), "张三");
    ASSERT_NEAR(cjson_object_get_number(user, "age", 0), 25, 0.001);
    
    cjson_value_t* contacts = cjson_object_get_array(user, "contacts");
    ASSERT_TRUE(contacts != NULL);
    ASSERT_EQ(cjson_array_size(contacts), 2);
    
    cjson_value_t* contact1 = cjson_array_get(contacts, 0);
    ASSERT_TRUE(contact1 != NULL);
    ASSERT_EQ_STR(cjson_object_get_string(contact1, "type", ""), "email");
    
    cjson_value_t* contact2 = cjson_array_get(contacts, 1);
    ASSERT_TRUE(contact2 != NULL);
    ASSERT_EQ_STR(cjson_object_get_string(contact2, "type", ""), "phone");
    
    cjson_free(root);
}

TEST(array_of_arrays) {
    cjson_value_t* root = cjson_create_array();
    
    cjson_value_t* arr1 = cjson_create_array();
    cjson_array_add(arr1, cjson_create_number(1));
    cjson_array_add(arr1, cjson_create_number(2));
    
    cjson_value_t* arr2 = cjson_create_array();
    cjson_array_add(arr2, cjson_create_number(3));
    cjson_array_add(arr2, cjson_create_number(4));
    
    cjson_array_add(root, arr1);
    cjson_array_add(root, arr2);
    
    ASSERT_EQ(cjson_array_size(root), 2);
    
    cjson_value_t* retrieved_arr1 = cjson_array_get(root, 0);
    ASSERT_TRUE(retrieved_arr1 != NULL);
    ASSERT_EQ(cjson_array_size(retrieved_arr1), 2);
    ASSERT_NEAR(cjson_get_number(cjson_array_get(retrieved_arr1, 0)), 1, 0.001);
    
    cjson_free(root);
}

TEST(type_check_null) {
    cjson_value_t* value = cjson_create_null();
    ASSERT_TRUE(cjson_is_null(value));
    ASSERT_TRUE(!cjson_is_bool(value));
    ASSERT_TRUE(!cjson_is_number(value));
    ASSERT_TRUE(!cjson_is_string(value));
    ASSERT_TRUE(!cjson_is_array(value));
    ASSERT_TRUE(!cjson_is_object(value));
    cjson_free(value);
}

TEST(type_check_null_input) {
    ASSERT_TRUE(!cjson_is_null(NULL));
    ASSERT_TRUE(!cjson_is_bool(NULL));
    ASSERT_TRUE(!cjson_is_number(NULL));
    ASSERT_TRUE(!cjson_is_string(NULL));
    ASSERT_TRUE(!cjson_is_array(NULL));
    ASSERT_TRUE(!cjson_is_object(NULL));
    ASSERT_EQ(cjson_get_type(NULL), CJSON_WRAP_TYPE_INVALID);
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    cJSON封装库测试用例                         \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("错误处理测试:\n");
    RUN_TEST(error_string);
    
    printf("\n解析测试:\n");
    RUN_TEST(parse_valid_json);
    RUN_TEST(parse_invalid_json);
    RUN_TEST(parse_null_input);
    RUN_TEST(parse_with_length);
    
    printf("\n创建测试:\n");
    RUN_TEST(create_null);
    RUN_TEST(create_bool);
    RUN_TEST(create_number);
    RUN_TEST(create_string);
    RUN_TEST(create_array);
    RUN_TEST(create_object);
    
    printf("\n数组操作测试:\n");
    RUN_TEST(array_operations);
    
    printf("\n对象操作测试:\n");
    RUN_TEST(object_operations);
    RUN_TEST(object_convenience_functions);
    
    printf("\n序列化测试:\n");
    RUN_TEST(print_json);
    
    printf("\n文件操作测试:\n");
    RUN_TEST(file_operations);
    
    printf("\n深拷贝和比较测试:\n");
    RUN_TEST(deep_copy);
    RUN_TEST(equal);
    
    printf("\n嵌套结构测试:\n");
    RUN_TEST(nested_structures);
    RUN_TEST(array_of_arrays);
    
    printf("\n类型检查测试:\n");
    RUN_TEST(type_check_null);
    RUN_TEST(type_check_null_input);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("测试结果: 通过 %d, 失败 %d\n", tests_passed, tests_failed);
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return tests_failed > 0 ? 1 : 0;
}
