/**
 * XML解析演示程序
 *
 * 功能：
 * - XML标签内容提取
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/xml_parse.h"

// 演示 1: 基本标签内容提取
static void demo_basic_extract(void) {
    printf("\n=== 演示 1: 基本标签内容提取 ===\n");

    const char *xml = "<root><name>John</name><age>30</age></root>";
    char buffer[64];

    // 提取name标签内容
    const char *result = xml_get_tag_content(xml, "name", buffer, sizeof(buffer));
    if (result) {
        printf("提取name标签: %s\n", buffer);
    } else {
        printf("提取name标签失败\n");
    }

    // 提取age标签内容
    result = xml_get_tag_content(xml, "age", buffer, sizeof(buffer));
    if (result) {
        printf("提取age标签: %s\n", buffer);
    } else {
        printf("提取age标签失败\n");
    }
}

// 演示 2: 嵌套标签提取
static void demo_nested_tags(void) {
    printf("\n=== 演示 2: 嵌套标签提取 ===\n");

    const char *xml = "<root>" 
                     "<person>" 
                     "<name>John</name>" 
                     "<age>30</age>" 
                     "<address>123 Main St</address>" 
                     "</person>" 
                     "</root>";
    char buffer[64];

    // 提取person标签内容
    const char *result = xml_get_tag_content(xml, "person", buffer, sizeof(buffer));
    if (result) {
        printf("提取person标签内容:\n  %s\n", buffer);
    } else {
        printf("提取person标签失败\n");
    }
}

// 演示 3: 复杂XML提取
static void demo_complex_xml(void) {
    printf("\n=== 演示 3: 复杂XML提取 ===\n");

    const char *xml = "<company>" 
                     "<name>Tech Corp</name>" 
                     "<employees>" 
                     "<employee id=\"1\">" 
                     "<name>John Doe</name>" 
                     "<position>Engineer</position>" 
                     "</employee>" 
                     "<employee id=\"2\">" 
                     "<name>Jane Smith</name>" 
                     "<position>Manager</position>" 
                     "</employee>" 
                     "</employees>" 
                     "</company>";
    char buffer[128];

    // 提取公司名称
    const char *result = xml_get_tag_content(xml, "name", buffer, sizeof(buffer));
    if (result) {
        printf("公司名称: %s\n", buffer);
    } else {
        printf("提取公司名称失败\n");
    }

    // 提取employees标签内容
    result = xml_get_tag_content(xml, "employees", buffer, sizeof(buffer));
    if (result) {
        printf("员工信息:\n  %s\n", buffer);
    } else {
        printf("提取员工信息失败\n");
    }
}

int main(void) {
    printf("========================================\n");
    printf("    XML解析演示\n");
    printf("========================================\n");

    demo_basic_extract();
    demo_nested_tags();
    demo_complex_xml();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
