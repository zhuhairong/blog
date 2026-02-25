/**
 * JSON 解析演示程序
 *
 * 功能：
 * - JSON 解析
 * - 数据类型处理
 * - 对象和数组操作
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/json.h"

// 演示 1: 基本解析
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本解析 ===\n");

    const char *json_text = "{\"name\":\"Alice\",\"age\":30,\"active\":true}";
    printf("JSON 文本: %s\n\n", json_text);

    json_value_t *root = json_parse(json_text);
    if (!root) {
        printf("解析失败\n");
        return;
    }

    printf("解析结果:\n");
    printf("  类型: %s\n", json_type(root) == JSON_OBJECT ? "对象" : "其他");

    json_value_t *name = json_object_get(root, "name");
    json_value_t *age = json_object_get(root, "age");
    json_value_t *active = json_object_get(root, "active");

    if (name) printf("  name: %s\n", json_as_string(name));
    if (age) printf("  age: %.0f\n", json_as_number(age));
    if (active) printf("  active: %s\n", json_as_bool(active) ? "true" : "false");

    json_free(root);
}

// 演示 2: 数组操作
static void demo_array(void) {
    printf("\n=== 演示 2: 数组操作 ===\n");

    const char *json_text = "[1, 2, 3, 4, 5]";
    printf("JSON 数组: %s\n\n", json_text);

    json_value_t *root = json_parse(json_text);
    if (!root) {
        printf("解析失败\n");
        return;
    }

    printf("数组大小: %zu\n", json_array_size(root));
    printf("数组元素:\n");

    for (size_t i = 0; i < json_array_size(root); i++) {
        json_value_t *item = json_array_get(root, i);
        printf("  [%zu] = %.0f\n", i, json_as_number(item));
    }

    json_free(root);
}

// 演示 3: 嵌套对象
static void demo_nested(void) {
    printf("\n=== 演示 3: 嵌套对象 ===\n");

    const char *json_text = "{"
        "\"user\":{"
            "\"name\":\"Bob\","
            "\"email\":\"bob@example.com\""
        "},"
        "\"address\":{"
            "\"city\":\"Beijing\","
            "\"zip\":\"100000\""
        "}"
    "}";

    printf("嵌套 JSON:\n%s\n\n", json_text);

    json_value_t *root = json_parse(json_text);
    if (!root) {
        printf("解析失败\n");
        return;
    }

    json_value_t *user = json_object_get(root, "user");
    json_value_t *address = json_object_get(root, "address");

    printf("用户信息:\n");
    if (user) {
        printf("  名称: %s\n", json_as_string(json_object_get(user, "name")));
        printf("  邮箱: %s\n", json_as_string(json_object_get(user, "email")));
    }

    printf("\n地址信息:\n");
    if (address) {
        printf("  城市: %s\n", json_as_string(json_object_get(address, "city")));
        printf("  邮编: %s\n", json_as_string(json_object_get(address, "zip")));
    }

    json_free(root);
}

// 演示 4: 复杂结构
static void demo_complex(void) {
    printf("\n=== 演示 4: 复杂结构 ===\n");

    const char *json_text = "{"
        "\"users\":["
            "{\"id\":1,\"name\":\"Alice\"},"
            "{\"id\":2,\"name\":\"Bob\"}"
        "],"
        "\"total\":2,"
        "\"page\":1"
    "}";

    printf("复杂 JSON:\n%s\n\n", json_text);

    json_value_t *root = json_parse(json_text);
    if (!root) {
        printf("解析失败\n");
        return;
    }

    json_value_t *users = json_object_get(root, "users");
    json_value_t *total = json_object_get(root, "total");
    json_value_t *page = json_object_get(root, "page");

    printf("分页信息:\n");
    printf("  总记录数: %.0f\n", json_as_number(total));
    printf("  当前页: %.0f\n", json_as_number(page));

    printf("\n用户列表:\n");
    if (users && json_type(users) == JSON_ARRAY) {
        for (size_t i = 0; i < json_array_size(users); i++) {
            json_value_t *user = json_array_get(users, i);
            json_value_t *id = json_object_get(user, "id");
            json_value_t *name = json_object_get(user, "name");

            printf("  用户 %zu: ID=%.0f, 名称=%s\n",
                   i + 1, json_as_number(id), json_as_string(name));
        }
    }

    json_free(root);
}

// 演示 5: 数据类型
static void demo_types(void) {
    printf("\n=== 演示 5: JSON 数据类型 ===\n");

    const char *types[] = {
        "null",
        "true",
        "false",
        "42",
        "3.14",
        "\"string\"",
        "[1,2,3]",
        "{\"key\":\"value\"}"
    };

    const char *type_names[] = {
        "NULL", "BOOL", "BOOL", "NUMBER", "NUMBER",
        "STRING", "ARRAY", "OBJECT"
    };

    printf("类型检测:\n");
    for (size_t i = 0; i < sizeof(types)/sizeof(types[0]); i++) {
        json_value_t *v = json_parse(types[i]);
        if (v) {
            printf("  %-15s -> %s\n", types[i], type_names[v->type]);
            json_free(v);
        }
    }
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. REST API\n");
    printf("   - 请求/响应数据格式\n");
    printf("   - 状态码和消息\n");
    printf("\n");

    printf("2. 配置文件\n");
    printf("   - 应用设置\n");
    printf("   - 用户偏好\n");
    printf("\n");

    printf("3. 数据交换\n");
    printf("   - 前后端通信\n");
    printf("   - 服务间通信\n");
    printf("\n");

    printf("4. 日志记录\n");
    printf("   - 结构化日志\n");
    printf("   - 便于分析处理\n");
}

int main(void) {
    printf("========================================\n");
    printf("    JSON 解析演示\n");
    printf("========================================\n");

    demo_basic();
    demo_array();
    demo_nested();
    demo_complex();
    demo_types();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
