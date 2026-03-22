#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson_wrapper.h"

static void demo_parse_json(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    JSON解析演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    const char* json_str = "{\n"
        "  \"name\": \"张三\",\n"
        "  \"age\": 25,\n"
        "  \"active\": true,\n"
        "  \"score\": 95.5,\n"
        "  \"tags\": [\"student\", \"developer\"],\n"
        "  \"address\": {\n"
        "    \"city\": \"北京\",\n"
        "    \"zip\": \"100000\"\n"
        "  }\n"
        "}";
    
    printf("解析JSON字符串:\n%s\n\n", json_str);
    
    cjson_value_t* root = cjson_parse(json_str);
    if (!root) {
        printf("解析失败: %s\n", cjson_get_error());
        return;
    }
    
    printf("解析结果:\n");
    
    const char* name = cjson_object_get_string(root, "name", "");
    printf("  name: %s\n", name);
    
    double age = cjson_object_get_number(root, "age", 0);
    printf("  age: %.0f\n", age);
    
    bool active = cjson_object_get_bool(root, "active", false);
    printf("  active: %s\n", active ? "true" : "false");
    
    double score = cjson_object_get_number(root, "score", 0);
    printf("  score: %.1f\n", score);
    
    cjson_value_t* tags = cjson_object_get_array(root, "tags");
    if (tags) {
        printf("  tags: [");
        size_t size = cjson_array_size(tags);
        for (size_t i = 0; i < size; i++) {
            cjson_value_t* tag = cjson_array_get(tags, i);
            printf("\"%s\"", cjson_get_string(tag));
            if (i < size - 1) printf(", ");
        }
        printf("]\n");
    }
    
    cjson_value_t* address = cjson_object_get_object(root, "address");
    if (address) {
        printf("  address:\n");
        printf("    city: %s\n", cjson_object_get_string(address, "city", ""));
        printf("    zip: %s\n", cjson_object_get_string(address, "zip", ""));
    }
    
    cjson_free(root);
}

static void demo_create_json(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    JSON创建演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    cjson_value_t* root = cjson_create_object();
    if (!root) {
        printf("创建对象失败\n");
        return;
    }
    
    printf("创建JSON对象:\n");
    
    cjson_object_set_string(root, "name", "李四");
    printf("  设置 name = \"李四\" ✓\n");
    
    cjson_object_set_number(root, "age", 30);
    printf("  设置 age = 30 ✓\n");
    
    cjson_object_set_bool(root, "active", false);
    printf("  设置 active = false ✓\n");
    
    cjson_object_set_null(root, "deleted");
    printf("  设置 deleted = null ✓\n");
    
    cjson_value_t* hobbies = cjson_create_array();
    cjson_array_add(hobbies, cjson_create_string("reading"));
    cjson_array_add(hobbies, cjson_create_string("gaming"));
    cjson_array_add(hobbies, cjson_create_string("coding"));
    cjson_object_set(root, "hobbies", hobbies);
    printf("  设置 hobbies = [\"reading\", \"gaming\", \"coding\"] ✓\n");
    
    cjson_value_t* scores = cjson_create_array();
    cjson_array_add(scores, cjson_create_number(85));
    cjson_array_add(scores, cjson_create_number(90));
    cjson_array_add(scores, cjson_create_number(95));
    cjson_object_set(root, "scores", scores);
    printf("  设置 scores = [85, 90, 95] ✓\n");
    
    printf("\n格式化输出:\n");
    char* json_str = cjson_print(root);
    if (json_str) {
        printf("%s\n", json_str);
        free(json_str);
    }
    
    printf("\n紧凑输出:\n");
    json_str = cjson_print_unformatted(root);
    if (json_str) {
        printf("%s\n", json_str);
        free(json_str);
    }
    
    cjson_free(root);
}

static void demo_array_operations(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    数组操作演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    cjson_value_t* arr = cjson_create_array();
    printf("创建空数组\n");
    
    printf("\n添加元素:\n");
    cjson_array_add(arr, cjson_create_number(1));
    printf("  添加 1, 大小 = %zu\n", cjson_array_size(arr));
    
    cjson_array_add(arr, cjson_create_number(2));
    printf("  添加 2, 大小 = %zu\n", cjson_array_size(arr));
    
    cjson_array_add(arr, cjson_create_number(3));
    printf("  添加 3, 大小 = %zu\n", cjson_array_size(arr));
    
    printf("\n遍历数组:\n");
    printf("  [");
    for (size_t i = 0; i < cjson_array_size(arr); i++) {
        cjson_value_t* item = cjson_array_get(arr, i);
        printf("%.0f", cjson_get_number(item));
        if (i < cjson_array_size(arr) - 1) printf(", ");
    }
    printf("]\n");
    
    printf("\n修改元素:\n");
    cjson_array_set(arr, 1, cjson_create_number(20));
    printf("  设置索引1为20\n");
    
    printf("  [");
    for (size_t i = 0; i < cjson_array_size(arr); i++) {
        cjson_value_t* item = cjson_array_get(arr, i);
        printf("%.0f", cjson_get_number(item));
        if (i < cjson_array_size(arr) - 1) printf(", ");
    }
    printf("]\n");
    
    printf("\n删除元素:\n");
    cjson_array_remove(arr, 0);
    printf("  删除索引0\n");
    
    printf("  [");
    for (size_t i = 0; i < cjson_array_size(arr); i++) {
        cjson_value_t* item = cjson_array_get(arr, i);
        printf("%.0f", cjson_get_number(item));
        if (i < cjson_array_size(arr) - 1) printf(", ");
    }
    printf("]\n");
    
    cjson_free(arr);
}

static void demo_object_operations(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    对象操作演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    cjson_value_t* obj = cjson_create_object();
    printf("创建空对象\n");
    
    printf("\n添加键值对:\n");
    cjson_object_set_string(obj, "key1", "value1");
    printf("  添加 key1 = \"value1\"\n");
    
    cjson_object_set_number(obj, "key2", 100);
    printf("  添加 key2 = 100\n");
    
    cjson_object_set_bool(obj, "key3", true);
    printf("  添加 key3 = true\n");
    
    printf("\n检查键是否存在:\n");
    printf("  key1 存在: %s\n", cjson_object_has(obj, "key1") ? "是" : "否");
    printf("  key4 存在: %s\n", cjson_object_has(obj, "key4") ? "是" : "否");
    
    printf("\n获取值:\n");
    printf("  key1: %s\n", cjson_object_get_string(obj, "key1", ""));
    printf("  key2: %.0f\n", cjson_object_get_number(obj, "key2", 0));
    printf("  key3: %s\n", cjson_object_get_bool(obj, "key3", false) ? "true" : "false");
    printf("  key4 (默认): %s\n", cjson_object_get_string(obj, "key4", "default_value"));
    
    printf("\n删除键:\n");
    cjson_object_remove(obj, "key2");
    printf("  删除 key2\n");
    printf("  key2 存在: %s\n", cjson_object_has(obj, "key2") ? "是" : "否");
    
    printf("\n对象大小: %zu\n", cjson_object_size(obj));
    
    cjson_free(obj);
}

static void demo_file_operations(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    文件读写演示                                \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    cjson_value_t* root = cjson_create_object();
    cjson_object_set_string(root, "title", "配置文件");
    cjson_object_set_number(root, "version", 1.0);
    cjson_object_set_bool(root, "debug", true);
    
    const char* filename = "/tmp/config.json";
    
    printf("保存JSON到文件: %s\n", filename);
    int result = cjson_print_to_file(root, filename);
    if (result == CJSON_WRAP_OK) {
        printf("保存成功 ✓\n");
    } else {
        printf("保存失败: %s\n", cjson_error_string(result));
    }
    
    printf("\n从文件加载JSON:\n");
    cjson_value_t* loaded = cjson_parse_file(filename);
    if (loaded) {
        printf("加载成功 ✓\n");
        printf("  title: %s\n", cjson_object_get_string(loaded, "title", ""));
        printf("  version: %.1f\n", cjson_object_get_number(loaded, "version", 0));
        printf("  debug: %s\n", cjson_object_get_bool(loaded, "debug", false) ? "true" : "false");
        cjson_free(loaded);
    } else {
        printf("加载失败: %s\n", cjson_get_error());
    }
    
    cjson_free(root);
}

static void demo_copy_compare(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    深拷贝和比较演示                            \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    cjson_value_t* obj1 = cjson_create_object();
    cjson_object_set_string(obj1, "name", "test");
    cjson_object_set_number(obj1, "value", 42);
    
    printf("原始对象:\n");
    char* str1 = cjson_print(obj1);
    printf("%s\n", str1);
    free(str1);
    
    printf("\n深拷贝对象:\n");
    cjson_value_t* obj2 = cjson_deep_copy(obj1);
    str1 = cjson_print(obj2);
    printf("%s\n", str1);
    free(str1);
    
    printf("\n比较对象:\n");
    printf("  obj1 == obj2: %s\n", cjson_equal(obj1, obj2) ? "是" : "否");
    
    printf("\n修改obj2:\n");
    cjson_object_set_number(obj2, "value", 100);
    str1 = cjson_print(obj2);
    printf("%s\n", str1);
    free(str1);
    
    printf("\n再次比较:\n");
    printf("  obj1 == obj2: %s\n", cjson_equal(obj1, obj2) ? "是" : "否");
    
    cjson_free(obj1);
    cjson_free(obj2);
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    cJSON封装库演示程序                         \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    demo_parse_json();
    demo_create_json();
    demo_array_operations();
    demo_object_operations();
    demo_file_operations();
    demo_copy_compare();
    
    printf("\n演示完成！\n\n");
    return 0;
}
