#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toml_parse.h"

void demo_toml_get_string(void) {
    printf("\n=== TOML字符串获取演示 ===\n");
    
    const char* toml_content = 
        "[database]\n"
        "host = \"localhost\"\n"
        "port = 5432\n"
        "name = \"mydb\"\n"
        "\n"
        "[server]\n"
        "address = \"127.0.0.1\"\n"
        "port = 8080\n";
    
    printf("TOML内容:\n%s\n", toml_content);
    
    char value[256] = {0};
    const char* result;
    
    result = toml_get_string(toml_content, "host", value, sizeof(value));
    if (result) {
        printf("host: %s\n", value);
    }
    
    result = toml_get_string(toml_content, "name", value, sizeof(value));
    if (result) {
        printf("name: %s\n", value);
    }
    
    result = toml_get_string(toml_content, "address", value, sizeof(value));
    if (result) {
        printf("address: %s\n", value);
    }
}

void demo_toml_config_parsing(void) {
    printf("\n=== TOML配置解析演示 ===\n");
    
    const char* config_toml = 
        "[application]\n"
        "name = \"MyApp\"\n"
        "version = \"1.0.0\"\n"
        "author = \"Developer\"\n"
        "\n"
        "[database]\n"
        "driver = \"postgresql\"\n"
        "host = \"localhost\"\n"
        "database = \"mydb\"\n";
    
    printf("配置文件内容:\n%s\n", config_toml);
    
    char value[256] = {0};
    
    if (toml_get_string(config_toml, "name", value, sizeof(value))) {
        printf("应用名称: %s\n", value);
    }
    
    if (toml_get_string(config_toml, "version", value, sizeof(value))) {
        printf("版本: %s\n", value);
    }
    
    if (toml_get_string(config_toml, "driver", value, sizeof(value))) {
        printf("数据库驱动: %s\n", value);
    }
    
    if (toml_get_string(config_toml, "host", value, sizeof(value))) {
        printf("数据库主机: %s\n", value);
    }
}

void demo_toml_missing_key(void) {
    printf("\n=== TOML缺失键演示 ===\n");
    
    const char* toml_content = 
        "name = \"test\"\n"
        "value = \"example\"\n";
    
    printf("TOML内容:\n%s\n", toml_content);
    
    char value[256] = {0};
    
    if (toml_get_string(toml_content, "name", value, sizeof(value))) {
        printf("找到 name: %s\n", value);
    }
    
    if (toml_get_string(toml_content, "missing", value, sizeof(value))) {
        printf("找到 missing: %s\n", value);
    } else {
        printf("未找到 missing 键\n");
    }
}

void demo_toml_special_values(void) {
    printf("\n=== TOML特殊值演示 ===\n");
    
    const char* toml_content = 
        "empty = \"\"\n"
        "spaces = \"  hello world  \"\n"
        "special = \"value with \\\"quotes\\\"\"\n"
        "path = \"/usr/local/bin\"\n";
    
    printf("TOML内容:\n%s\n", toml_content);
    
    char value[256] = {0};
    
    if (toml_get_string(toml_content, "empty", value, sizeof(value))) {
        printf("empty: \"%s\" (空字符串)\n", value);
    }
    
    if (toml_get_string(toml_content, "spaces", value, sizeof(value))) {
        printf("spaces: \"%s\"\n", value);
    }
    
    if (toml_get_string(toml_content, "path", value, sizeof(value))) {
        printf("path: %s\n", value);
    }
}

int main(void) {
    printf("========================================\n");
    printf("    TOML解析演示程序\n");
    printf("========================================\n");
    
    demo_toml_get_string();
    demo_toml_config_parsing();
    demo_toml_missing_key();
    demo_toml_special_values();
    
    printf("\n=== TOML解析演示完成 ===\n");
    return 0;
}
