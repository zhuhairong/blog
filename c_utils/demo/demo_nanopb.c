#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nanopb_wrapper.h"

typedef struct {
    int32_t id;
    char* name;
    int32_t age;
    float score;
    bool active;
} person_t;

static const npb_field_desc_t person_fields[] = {
    {1, NPB_TYPE_INT32, "id", offsetof(person_t, id), sizeof(int32_t)},
    {2, NPB_TYPE_STRING, "name", offsetof(person_t, name), sizeof(char*)},
    {3, NPB_TYPE_INT32, "age", offsetof(person_t, age), sizeof(int32_t)},
    {4, NPB_TYPE_FLOAT, "score", offsetof(person_t, score), sizeof(float)},
    {5, NPB_TYPE_BOOL, "active", offsetof(person_t, active), sizeof(bool)},
};

static const npb_msgdesc_t person_desc = {
    "person_t",
    person_fields,
    sizeof(person_fields) / sizeof(person_fields[0]),
    sizeof(person_t)
};

static void demo_basic_types(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    基本类型编解码演示                           \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    npb_encoder_t* enc = npb_encoder_create();
    if (!enc) {
        printf("创建编码器失败\n");
        return;
    }
    
    printf("编码基本类型:\n");
    
    npb_error_t err;
    
    err = npb_encode_tag(enc, 1, 0);
    printf("  字段1 (int32): ");
    err = npb_encode_varint(enc, 12345);
    printf("12345 %s\n", err == NPB_OK ? "✓" : "✗");
    
    err = npb_encode_tag(enc, 2, 1);
    printf("  字段2 (double): ");
    err = npb_encode_double(enc, 3.14159265358979);
    printf("3.14159265358979 %s\n", err == NPB_OK ? "✓" : "✗");
    
    err = npb_encode_tag(enc, 3, 2);
    printf("  字段3 (string): ");
    err = npb_encode_string(enc, "Hello, Nanopb!");
    printf("\"Hello, Nanopb!\" %s\n", err == NPB_OK ? "✓" : "✗");
    
    err = npb_encode_tag(enc, 4, 5);
    printf("  字段4 (float): ");
    err = npb_encode_float(enc, 2.71828f);
    printf("2.71828 %s\n", err == NPB_OK ? "✓" : "✗");
    
    err = npb_encode_tag(enc, 5, 0);
    printf("  字段5 (bool): ");
    err = npb_encode_varint(enc, 1);
    printf("true %s\n", err == NPB_OK ? "✓" : "✗");
    
    size_t size;
    const uint8_t* buffer = npb_encoder_buffer(enc, &size);
    printf("\n编码后大小: %zu 字节\n", size);
    
    printf("\n解码基本类型:\n");
    
    npb_decoder_t* dec = npb_decoder_create(buffer, size);
    if (!dec) {
        printf("创建解码器失败\n");
        npb_encoder_destroy(enc);
        return;
    }
    
    uint32_t field_id;
    uint8_t wire_type;
    
    while (npb_decoder_remaining(dec) > 0) {
        err = npb_decode_tag(dec, &field_id, &wire_type);
        if (err != NPB_OK) break;
        
        switch (field_id) {
            case 1: {
                uint64_t value;
                err = npb_decode_varint(dec, &value);
                printf("  字段1 (int32): %lu %s\n", (unsigned long)value, err == NPB_OK ? "✓" : "✗");
                break;
            }
            case 2: {
                double value;
                err = npb_decode_double(dec, &value);
                printf("  字段2 (double): %.15g %s\n", value, err == NPB_OK ? "✓" : "✗");
                break;
            }
            case 3: {
                char value[256];
                err = npb_decode_string(dec, value, sizeof(value));
                printf("  字段3 (string): \"%s\" %s\n", value, err == NPB_OK ? "✓" : "✗");
                break;
            }
            case 4: {
                float value;
                err = npb_decode_float(dec, &value);
                printf("  字段4 (float): %.5g %s\n", value, err == NPB_OK ? "✓" : "✗");
                break;
            }
            case 5: {
                uint64_t value;
                err = npb_decode_varint(dec, &value);
                printf("  字段5 (bool): %s %s\n", value ? "true" : "false", err == NPB_OK ? "✓" : "✗");
                break;
            }
            default:
                printf("  未知字段: %u\n", field_id);
        }
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

static void demo_message_encoding(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    消息编解码演示                               \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    person_t person = {
        .id = 1001,
        .name = strdup("张三"),
        .age = 25,
        .score = 95.5f,
        .active = true
    };
    
    printf("原始消息:\n");
    printf("  id: %d\n", person.id);
    printf("  name: %s\n", person.name);
    printf("  age: %d\n", person.age);
    printf("  score: %.1f\n", person.score);
    printf("  active: %s\n", person.active ? "true" : "false");
    
    uint8_t buffer[256];
    size_t size = sizeof(buffer);
    
    int result = npb_message_encode(&person, &person_desc, buffer, &size);
    if (result != 0) {
        printf("编码失败\n");
        free(person.name);
        return;
    }
    
    printf("\n编码后大小: %zu 字节\n", size);
    
    person_t decoded = {0};
    result = npb_message_decode(buffer, size, &decoded, &person_desc);
    if (result != 0) {
        printf("解码失败\n");
        free(person.name);
        return;
    }
    
    printf("\n解码后消息:\n");
    printf("  id: %d\n", decoded.id);
    printf("  name: %s\n", decoded.name ? decoded.name : "(null)");
    printf("  age: %d\n", decoded.age);
    printf("  score: %.1f\n", decoded.score);
    printf("  active: %s\n", decoded.active ? "true" : "false");
    
    free(person.name);
    if (decoded.name) free(decoded.name);
}

static void demo_field_accessors(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    字段访问器演示                               \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    person_t person = {0};
    
    printf("使用字段访问器设置值:\n");
    
    npb_set_int32(&person, &person_desc, 1, 2001);
    printf("  设置 id = 2001\n");
    
    npb_set_string(&person, &person_desc, 2, "李四");
    printf("  设置 name = \"李四\"\n");
    
    npb_set_int32(&person, &person_desc, 3, 30);
    printf("  设置 age = 30\n");
    
    npb_set_float(&person, &person_desc, 4, 88.5f);
    printf("  设置 score = 88.5\n");
    
    npb_set_bool(&person, &person_desc, 5, false);
    printf("  设置 active = false\n");
    
    printf("\n使用字段访问器读取值:\n");
    
    int32_t id;
    npb_get_int32(&person, &person_desc, 1, &id);
    printf("  id: %d\n", id);
    
    char name[64];
    npb_get_string(&person, &person_desc, 2, name, sizeof(name));
    printf("  name: %s\n", name);
    
    int32_t age;
    npb_get_int32(&person, &person_desc, 3, &age);
    printf("  age: %d\n", age);
    
    float score;
    npb_get_float(&person, &person_desc, 4, &score);
    printf("  score: %.1f\n", score);
    
    bool active;
    npb_get_bool(&person, &person_desc, 5, &active);
    printf("  active: %s\n", active ? "true" : "false");
    
    if (person.name) free(person.name);
}

static void demo_file_io(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    文件读写演示                                 \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    person_t person = {
        .id = 3001,
        .name = strdup("王五"),
        .age = 28,
        .score = 92.0f,
        .active = true
    };
    
    const char* filename = "/tmp/person.bin";
    
    printf("保存消息到文件: %s\n", filename);
    int result = npb_message_to_file(&person, &person_desc, filename);
    if (result != 0) {
        printf("保存失败\n");
        free(person.name);
        return;
    }
    printf("保存成功\n");
    
    person_t loaded = {0};
    printf("\n从文件加载消息:\n");
    result = npb_message_from_file(filename, &loaded, &person_desc);
    if (result != 0) {
        printf("加载失败\n");
        free(person.name);
        return;
    }
    
    printf("  id: %d\n", loaded.id);
    printf("  name: %s\n", loaded.name ? loaded.name : "(null)");
    printf("  age: %d\n", loaded.age);
    printf("  score: %.1f\n", loaded.score);
    printf("  active: %s\n", loaded.active ? "true" : "false");
    
    free(person.name);
    if (loaded.name) free(loaded.name);
}

static void demo_zigzag(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    ZigZag编码演示                               \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int32_t values[] = {0, -1, 1, -2, 2, -100, 100, -1000, 1000};
    int count = sizeof(values) / sizeof(values[0]);
    
    printf("值          ZigZag编码    解码后\n");
    printf("───────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < count; i++) {
        uint32_t encoded = npb_zigzag_encode(values[i]);
        int32_t decoded = npb_zigzag_decode(encoded);
        printf("%-11d %-12u %d %s\n", values[i], encoded, decoded, 
               decoded == values[i] ? "✓" : "✗");
    }
}

static void demo_varint_size(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    Varint大小计算演示                           \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    uint64_t values[] = {0, 127, 128, 16383, 16384, 2097151, 2097152, 
                         268435455, 268435456, UINT64_MAX};
    int count = sizeof(values) / sizeof(values[0]);
    
    printf("值                    编码字节数\n");
    printf("───────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < count; i++) {
        size_t size = npb_varint_size(values[i]);
        printf("%-20lu %zu 字节\n", (unsigned long)values[i], size);
    }
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    Nanopb封装库演示程序                         \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    demo_basic_types();
    demo_message_encoding();
    demo_field_accessors();
    demo_file_io();
    demo_zigzag();
    demo_varint_size();
    
    printf("\n演示完成！\n\n");
    return 0;
}
