#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "nanopb_wrapper.h"

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

TEST(encoder_create_destroy) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    ASSERT_TRUE(buf != NULL);
    ASSERT_EQ(size, 0);
    
    npb_encoder_destroy(enc);
}

TEST(encoder_with_buffer) {
    uint8_t buffer[100];
    npb_encoder_t* enc = npb_encoder_create_with_buffer(buffer, sizeof(buffer));
    ASSERT_TRUE(enc != NULL);
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    ASSERT_TRUE(buf == buffer);
    
    npb_encoder_destroy(enc);
}

TEST(encoder_reset) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    npb_encode_varint(enc, 12345);
    
    size_t size;
    npb_encoder_buffer(enc, &size);
    ASSERT_TRUE(size > 0);
    
    npb_encoder_reset(enc);
    npb_encoder_buffer(enc, &size);
    ASSERT_EQ(size, 0);
    
    npb_encoder_destroy(enc);
}

TEST(decoder_create_destroy) {
    uint8_t data[] = {0x08, 0x96, 0x01};
    npb_decoder_t* dec = npb_decoder_create(data, sizeof(data));
    ASSERT_TRUE(dec != NULL);
    
    ASSERT_EQ(npb_decoder_remaining(dec), sizeof(data));
    
    npb_decoder_destroy(dec);
}

TEST(varint_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    uint64_t values[] = {0, 1, 127, 128, 300, 16384, 2097152, 268435456, 
                         (uint64_t)-1, (uint64_t)-2};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        npb_error_t err = npb_encode_varint(enc, values[i]);
        ASSERT_EQ(err, NPB_OK);
    }
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    for (int i = 0; i < count; i++) {
        uint64_t value;
        npb_error_t err = npb_decode_varint(dec, &value);
        ASSERT_EQ(err, NPB_OK);
        ASSERT_EQ(value, values[i]);
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(fixed32_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    uint32_t values[] = {0, 1, 255, 256, 65535, 65536, 0xFFFFFFFF, 0x12345678};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        npb_error_t err = npb_encode_fixed32(enc, values[i]);
        ASSERT_EQ(err, NPB_OK);
    }
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    for (int i = 0; i < count; i++) {
        uint32_t value;
        npb_error_t err = npb_decode_fixed32(dec, &value);
        ASSERT_EQ(err, NPB_OK);
        ASSERT_EQ(value, values[i]);
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(fixed64_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    uint64_t values[] = {0, 1, 0xFFFFFFFF, (uint64_t)0xFFFFFFFFFFFFFFFF, 0x123456789ABCDEF0ULL};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        npb_error_t err = npb_encode_fixed64(enc, values[i]);
        ASSERT_EQ(err, NPB_OK);
    }
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    for (int i = 0; i < count; i++) {
        uint64_t value;
        npb_error_t err = npb_decode_fixed64(dec, &value);
        ASSERT_EQ(err, NPB_OK);
        ASSERT_EQ(value, values[i]);
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(float_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    float values[] = {0.0f, 1.0f, -1.0f, 3.14159f, -3.14159f, 1e10f, 1e-10f};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        npb_error_t err = npb_encode_float(enc, values[i]);
        ASSERT_EQ(err, NPB_OK);
    }
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    for (int i = 0; i < count; i++) {
        float value;
        npb_error_t err = npb_decode_float(dec, &value);
        ASSERT_EQ(err, NPB_OK);
        ASSERT_NEAR(value, values[i], 1e-5f);
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(double_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    double values[] = {0.0, 1.0, -1.0, 3.14159265358979, -3.14159265358979, 1e100, 1e-100};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        npb_error_t err = npb_encode_double(enc, values[i]);
        ASSERT_EQ(err, NPB_OK);
    }
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    for (int i = 0; i < count; i++) {
        double value;
        npb_error_t err = npb_decode_double(dec, &value);
        ASSERT_EQ(err, NPB_OK);
        ASSERT_NEAR(value, values[i], 1e-10);
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(string_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    const char* values[] = {"", "a", "hello", "Hello, World!", "你好世界"};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        npb_error_t err = npb_encode_string(enc, values[i]);
        ASSERT_EQ(err, NPB_OK);
    }
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    for (int i = 0; i < count; i++) {
        char value[256];
        npb_error_t err = npb_decode_string(dec, value, sizeof(value));
        ASSERT_EQ(err, NPB_OK);
        ASSERT_EQ_STR(value, values[i]);
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(bytes_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    uint8_t data1[] = {0x00, 0x01, 0x02, 0x03, 0xFF};
    uint8_t data2[] = {};
    uint8_t data3[256];
    for (int i = 0; i < 256; i++) data3[i] = (uint8_t)i;
    
    npb_error_t err = npb_encode_bytes(enc, data1, sizeof(data1));
    ASSERT_EQ(err, NPB_OK);
    
    err = npb_encode_bytes(enc, data2, sizeof(data2));
    ASSERT_EQ(err, NPB_OK);
    
    err = npb_encode_bytes(enc, data3, sizeof(data3));
    ASSERT_EQ(err, NPB_OK);
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    uint8_t decoded[256];
    size_t len;
    
    err = npb_decode_bytes(dec, decoded, &len, sizeof(decoded));
    ASSERT_EQ(err, NPB_OK);
    ASSERT_EQ(len, sizeof(data1));
    ASSERT_TRUE(memcmp(decoded, data1, len) == 0);
    
    err = npb_decode_bytes(dec, decoded, &len, sizeof(decoded));
    ASSERT_EQ(err, NPB_OK);
    ASSERT_EQ(len, 0);
    
    err = npb_decode_bytes(dec, decoded, &len, sizeof(decoded));
    ASSERT_EQ(err, NPB_OK);
    ASSERT_EQ(len, 256);
    ASSERT_TRUE(memcmp(decoded, data3, len) == 0);
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(zigzag_encode_decode) {
    int32_t values[] = {0, -1, 1, -2, 2, -100, 100, -1000, 1000, 
                        INT32_MAX, INT32_MIN};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        uint32_t encoded = npb_zigzag_encode(values[i]);
        int32_t decoded = npb_zigzag_decode(encoded);
        ASSERT_EQ(decoded, values[i]);
    }
}

TEST(zigzag64_encode_decode) {
    int64_t values[] = {0, -1, 1, -2, 2, -100, 100, -1000, 1000,
                        INT64_MAX, INT64_MIN};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        uint64_t encoded = npb_zigzag_encode64(values[i]);
        int64_t decoded = npb_zigzag_decode64(encoded);
        ASSERT_EQ(decoded, values[i]);
    }
}

TEST(varint_size) {
    ASSERT_EQ(npb_varint_size(0), 1);
    ASSERT_EQ(npb_varint_size(127), 1);
    ASSERT_EQ(npb_varint_size(128), 2);
    ASSERT_EQ(npb_varint_size(16383), 2);
    ASSERT_EQ(npb_varint_size(16384), 3);
    ASSERT_EQ(npb_varint_size(2097151), 3);
    ASSERT_EQ(npb_varint_size(2097152), 4);
}

typedef struct {
    int32_t id;
    char* name;
    int32_t value;
} test_msg_t;

static const npb_field_desc_t test_msg_fields[] = {
    {1, NPB_TYPE_INT32, "id", offsetof(test_msg_t, id), sizeof(int32_t)},
    {2, NPB_TYPE_STRING, "name", offsetof(test_msg_t, name), sizeof(char*)},
    {3, NPB_TYPE_INT32, "value", offsetof(test_msg_t, value), sizeof(int32_t)},
};

static const npb_msgdesc_t test_msg_desc = {
    "test_msg_t",
    test_msg_fields,
    sizeof(test_msg_fields) / sizeof(test_msg_fields[0]),
    sizeof(test_msg_t)
};

TEST(message_encode_decode) {
    test_msg_t msg = {
        .id = 123,
        .name = strdup("test"),
        .value = 456
    };
    
    uint8_t buffer[256];
    size_t size = sizeof(buffer);
    
    int result = npb_message_encode(&msg, &test_msg_desc, buffer, &size);
    ASSERT_EQ(result, 0);
    ASSERT_TRUE(size > 0);
    
    test_msg_t decoded = {0};
    result = npb_message_decode(buffer, size, &decoded, &test_msg_desc);
    ASSERT_EQ(result, 0);
    
    ASSERT_EQ(decoded.id, msg.id);
    ASSERT_EQ(decoded.value, msg.value);
    
    free(msg.name);
    if (decoded.name) free(decoded.name);
}

TEST(field_accessors) {
    test_msg_t msg = {0};
    
    int result = npb_set_int32(&msg, &test_msg_desc, 1, 789);
    ASSERT_EQ(result, 0);
    
    int32_t id;
    result = npb_get_int32(&msg, &test_msg_desc, 1, &id);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(id, 789);
    
    result = npb_set_string(&msg, &test_msg_desc, 2, "accessor_test");
    ASSERT_EQ(result, 0);
    
    char name[64];
    result = npb_get_string(&msg, &test_msg_desc, 2, name, sizeof(name));
    ASSERT_EQ(result, 0);
    ASSERT_EQ_STR(name, "accessor_test");
    
    result = npb_set_int32(&msg, &test_msg_desc, 3, -100);
    ASSERT_EQ(result, 0);
    
    int32_t value;
    result = npb_get_int32(&msg, &test_msg_desc, 3, &value);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(value, -100);
    
    if (msg.name) free(msg.name);
}

TEST(error_handling) {
    const char* err_str = npb_error_string(NPB_OK);
    ASSERT_TRUE(err_str != NULL);
    
    err_str = npb_error_string(NPB_ERR_NULL_PTR);
    ASSERT_TRUE(err_str != NULL);
    
    err_str = npb_error_string(NPB_ERR_BUFFER_TOO_SMALL);
    ASSERT_TRUE(err_str != NULL);
    
    err_str = npb_error_string((npb_error_t)-999);
    ASSERT_TRUE(err_str != NULL);
}

TEST(buffer_too_small) {
    uint8_t small_buffer[1];
    npb_encoder_t* enc = npb_encoder_create_with_buffer(small_buffer, sizeof(small_buffer));
    ASSERT_TRUE(enc != NULL);
    
    npb_error_t err = npb_encode_fixed64(enc, 0x123456789ABCDEF0ULL);
    ASSERT_EQ(err, NPB_ERR_BUFFER_TOO_SMALL);
    
    npb_encoder_destroy(enc);
}

TEST(tag_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    npb_error_t err = npb_encode_tag(enc, 1, 0);
    ASSERT_EQ(err, NPB_OK);
    
    err = npb_encode_tag(enc, 100, 2);
    ASSERT_EQ(err, NPB_OK);
    
    err = npb_encode_tag(enc, 536870911, 5);
    ASSERT_EQ(err, NPB_OK);
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    uint32_t field_id;
    uint8_t wire_type;
    
    err = npb_decode_tag(dec, &field_id, &wire_type);
    ASSERT_EQ(err, NPB_OK);
    ASSERT_EQ(field_id, 1);
    ASSERT_EQ(wire_type, 0);
    
    err = npb_decode_tag(dec, &field_id, &wire_type);
    ASSERT_EQ(err, NPB_OK);
    ASSERT_EQ(field_id, 100);
    ASSERT_EQ(wire_type, 2);
    
    err = npb_decode_tag(dec, &field_id, &wire_type);
    ASSERT_EQ(err, NPB_OK);
    ASSERT_EQ(field_id, 536870911);
    ASSERT_EQ(wire_type, 5);
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(signed_varint_encode_decode) {
    npb_encoder_t* enc = npb_encoder_create();
    ASSERT_TRUE(enc != NULL);
    
    int64_t values[] = {0, 1, -1, 127, -127, 128, -128, 
                        INT64_MAX, INT64_MIN};
    int count = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < count; i++) {
        npb_error_t err = npb_encode_signed_varint(enc, values[i]);
        ASSERT_EQ(err, NPB_OK);
    }
    
    size_t size;
    const uint8_t* buf = npb_encoder_buffer(enc, &size);
    
    npb_decoder_t* dec = npb_decoder_create(buf, size);
    ASSERT_TRUE(dec != NULL);
    
    for (int i = 0; i < count; i++) {
        int64_t value;
        npb_error_t err = npb_decode_signed_varint(dec, &value);
        ASSERT_EQ(err, NPB_OK);
        ASSERT_EQ(value, values[i]);
    }
    
    npb_decoder_destroy(dec);
    npb_encoder_destroy(enc);
}

TEST(decoder_reset) {
    uint8_t data[] = {0x08, 0x96, 0x01, 0x10, 0xE8, 0x07};
    npb_decoder_t* dec = npb_decoder_create(data, sizeof(data));
    ASSERT_TRUE(dec != NULL);
    
    uint64_t value;
    npb_error_t err = npb_decode_varint(dec, &value);
    ASSERT_EQ(err, NPB_OK);
    
    npb_decoder_reset(dec, data, sizeof(data));
    ASSERT_EQ(npb_decoder_remaining(dec), sizeof(data));
    
    npb_decoder_destroy(dec);
}

TEST(null_pointer_handling) {
    ASSERT_TRUE(npb_encoder_create_with_buffer(NULL, 100) == NULL);
    ASSERT_TRUE(npb_decoder_create(NULL, 10) == NULL);
    
    ASSERT_EQ(npb_encode_varint(NULL, 123), NPB_ERR_NULL_PTR);
    ASSERT_EQ(npb_decode_varint(NULL, NULL), NPB_ERR_NULL_PTR);
}

TEST(file_io) {
    typedef struct {
        int32_t id;
        char* name;
        double value;
    } file_test_msg_t;
    
    static const npb_field_desc_t file_test_fields[] = {
        {1, NPB_TYPE_INT32, "id", offsetof(file_test_msg_t, id), sizeof(int32_t)},
        {2, NPB_TYPE_STRING, "name", offsetof(file_test_msg_t, name), sizeof(char*)},
        {3, NPB_TYPE_DOUBLE, "value", offsetof(file_test_msg_t, value), sizeof(double)},
    };
    
    static const npb_msgdesc_t file_test_desc = {
        "file_test_msg_t",
        file_test_fields,
        sizeof(file_test_fields) / sizeof(file_test_fields[0]),
        sizeof(file_test_msg_t)
    };
    
    file_test_msg_t msg = {
        .id = 999,
        .name = strdup("file_test"),
        .value = 3.14159265358979
    };
    
    const char* filename = "/tmp/npb_test.bin";
    int result = npb_message_to_file(&msg, &file_test_desc, filename);
    ASSERT_EQ(result, 0);
    
    file_test_msg_t loaded = {0};
    result = npb_message_from_file(filename, &loaded, &file_test_desc);
    ASSERT_EQ(result, 0);
    
    ASSERT_EQ(loaded.id, msg.id);
    ASSERT_NEAR(loaded.value, msg.value, 1e-10);
    
    free(msg.name);
    if (loaded.name) free(loaded.name);
}

int main(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    Nanopb封装库测试用例                         \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("编码器测试:\n");
    RUN_TEST(encoder_create_destroy);
    RUN_TEST(encoder_with_buffer);
    RUN_TEST(encoder_reset);
    
    printf("\n解码器测试:\n");
    RUN_TEST(decoder_create_destroy);
    RUN_TEST(decoder_reset);
    
    printf("\n基本类型编解码测试:\n");
    RUN_TEST(varint_encode_decode);
    RUN_TEST(signed_varint_encode_decode);
    RUN_TEST(fixed32_encode_decode);
    RUN_TEST(fixed64_encode_decode);
    RUN_TEST(float_encode_decode);
    RUN_TEST(double_encode_decode);
    RUN_TEST(string_encode_decode);
    RUN_TEST(bytes_encode_decode);
    
    printf("\nZigZag编码测试:\n");
    RUN_TEST(zigzag_encode_decode);
    RUN_TEST(zigzag64_encode_decode);
    
    printf("\nVarint大小测试:\n");
    RUN_TEST(varint_size);
    
    printf("\n消息编解码测试:\n");
    RUN_TEST(message_encode_decode);
    RUN_TEST(field_accessors);
    RUN_TEST(file_io);
    
    printf("\n错误处理测试:\n");
    RUN_TEST(error_handling);
    RUN_TEST(buffer_too_small);
    RUN_TEST(null_pointer_handling);
    
    printf("\nTag编解码测试:\n");
    RUN_TEST(tag_encode_decode);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("测试结果: 通过 %d, 失败 %d\n", tests_passed, tests_failed);
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return tests_failed > 0 ? 1 : 0;
}
