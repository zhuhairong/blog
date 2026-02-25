/**
 * @file demo_coap_pkt.c
 * @brief CoAP 协议数据包演示
 *
 * 本演示展示了 CoAP 数据包的创建、编码和解码
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/coap_pkt.h"

// 打印十六进制数据
static void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len && i < 32; i++) {
        printf("%02X ", data[i]);
    }
    if (len > 32) printf("...");
    printf("\n");
}

// 演示 1: 创建和编码 CoAP 消息
static void demo_create_encode(void) {
    printf("\n=== 演示 1: 创建和编码 CoAP 消息 ===\n");

    // 创建 GET 请求
    coap_pkt_t* pkt = coap_pkt_create();
    if (!pkt) {
        printf("创建 CoAP 消息失败\n");
        return;
    }

    // 设置消息字段
    pkt->ver = 1;
    pkt->type = COAP_TYPE_CON;
    pkt->code = COAP_METHOD_GET;
    pkt->msg_id = 0x1234;
    pkt->tkl = 0;

    // 添加选项
    const char* path = "temperature";
    coap_pkt_add_option(pkt, COAP_OPTION_URI_PATH, (const uint8_t*)path, strlen(path));

    printf("GET 请求:\n");
    printf("  类型: CON (Confirmable)\n");
    printf("  方法: GET\n");
    printf("  消息 ID: 0x%04X\n", pkt->msg_id);
    printf("  URI 路径: %s\n", path);

    // 编码
    uint8_t buffer[256];
    size_t written;
    coap_error_t err = coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);

    if (err == COAP_OK) {
        print_hex("  编码数据", buffer, written);
    } else {
        printf("  编码失败: %s\n", coap_strerror(err));
    }

    coap_pkt_free(pkt);

    // 创建 POST 请求
    pkt = coap_pkt_create();
    if (!pkt) return;

    pkt->ver = 1;
    pkt->type = COAP_TYPE_CON;
    pkt->code = COAP_METHOD_POST;
    pkt->msg_id = 0x1235;
    pkt->tkl = 0;

    const char* post_path = "sensors";
    coap_pkt_add_option(pkt, COAP_OPTION_URI_PATH, (const uint8_t*)post_path, strlen(post_path));

    const char* payload = "{\"temp\":25.5}";
    coap_pkt_set_payload(pkt, (const uint8_t*)payload, strlen(payload));

    printf("\nPOST 请求:\n");
    printf("  类型: CON\n");
    printf("  方法: POST\n");
    printf("  消息 ID: 0x%04X\n", pkt->msg_id);
    printf("  URI 路径: %s\n", post_path);
    printf("  负载: %s\n", payload);

    err = coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);
    if (err == COAP_OK) {
        print_hex("  编码数据", buffer, written);
    }

    coap_pkt_free(pkt);
}

// 演示 2: 解码 CoAP 消息
static void demo_decode(void) {
    printf("\n=== 演示 2: 解码 CoAP 消息 ===\n");

    // 构造一个简单的 CoAP 响应
    // Ver: 1, Type: ACK (2), TKL: 0, Code: 2.05 Content (70)
    uint8_t response[] = {
        0x60,  // Ver=1, Type=2 (ACK), TKL=0
        0x45,  // Code=2.05 (Content)
        0x12, 0x34,  // Message ID
        // 选项: Content-Format = text/plain (0)
        0xFF,  // Payload marker
        'O', 'K'  // Payload
    };

    print_hex("原始数据", response, sizeof(response));

    coap_pkt_t* pkt = coap_pkt_create();
    if (!pkt) return;

    coap_error_t err = coap_pkt_decode(response, sizeof(response), pkt);

    if (err == COAP_OK) {
        printf("\n解码结果:\n");
        printf("  版本: %d\n", pkt->ver);
        printf("  类型: %d (%s)\n", pkt->type,
               pkt->type == COAP_TYPE_ACK ? "ACK" :
               pkt->type == COAP_TYPE_CON ? "CON" :
               pkt->type == COAP_TYPE_NON ? "NON" : "RST");
        printf("  代码: %d\n", pkt->code);
        printf("  消息 ID: 0x%04X\n", pkt->msg_id);
        printf("  Token 长度: %d\n", pkt->tkl);

        if (pkt->payload_len > 0) {
            printf("  负载 (%zu bytes): ", pkt->payload_len);
            for (size_t i = 0; i < pkt->payload_len; i++) {
                printf("%c", pkt->payload[i]);
            }
            printf("\n");
        }
    } else {
        printf("解码失败: %s\n", coap_strerror(err));
    }

    coap_pkt_free(pkt);
}

// 演示 3: 选项操作
static void demo_options(void) {
    printf("\n=== 演示 3: 选项操作 ===\n");

    coap_pkt_t* pkt = coap_pkt_create();
    if (!pkt) return;

    // 添加多个选项
    const char* path1 = "api";
    const char* path2 = "v1";
    const char* path3 = "devices";

    coap_pkt_add_option(pkt, COAP_OPTION_URI_PATH, (const uint8_t*)path1, strlen(path1));
    coap_pkt_add_option(pkt, COAP_OPTION_URI_PATH, (const uint8_t*)path2, strlen(path2));
    coap_pkt_add_option(pkt, COAP_OPTION_URI_PATH, (const uint8_t*)path3, strlen(path3));

    uint16_t port = 5683;
    coap_pkt_add_option(pkt, COAP_OPTION_URI_PORT, (const uint8_t*)&port, sizeof(port));

    printf("添加的选项:\n");
    printf("  URI-Path: %s\n", path1);
    printf("  URI-Path: %s\n", path2);
    printf("  URI-Path: %s\n", path3);
    printf("  URI-Port: %d\n", port);

    // 编码并解码
    uint8_t buffer[256];
    size_t written;
    coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);
    coap_pkt_free(pkt);

    // 解码并获取选项
    pkt = coap_pkt_create();
    coap_pkt_decode(buffer, written, pkt);

    printf("\n解码后的选项:\n");
    coap_option_s* opt = pkt->options;
    int count = 0;
    while (opt) {
        printf("  选项 %d: 编号=%d, 长度=%d\n",
               ++count, opt->number, opt->length);
        opt = opt->next;
    }

    coap_pkt_free(pkt);
}

// 演示 4: 消息类型
static void demo_message_types(void) {
    printf("\n=== 演示 4: 消息类型 ===\n");

    const char* type_names[] = {"CON", "NON", "ACK", "RST"};
    coap_type_t types[] = {COAP_TYPE_CON, COAP_TYPE_NON, COAP_TYPE_ACK, COAP_TYPE_RST};

    printf("CoAP 消息类型:\n\n");

    for (int i = 0; i < 4; i++) {
        coap_pkt_t* pkt = coap_pkt_create();
        if (!pkt) continue;

        pkt->ver = 1;
        pkt->type = types[i];
        pkt->code = COAP_METHOD_GET;
        pkt->msg_id = 0x1000 + i;
        pkt->tkl = 0;

        uint8_t buffer[64];
        size_t written;
        coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);

        printf("%s (%d): ", type_names[i], types[i]);
        print_hex("", buffer, written > 4 ? 4 : written);

        coap_pkt_free(pkt);
    }

    printf("\n说明:\n");
    printf("  CON - Confirmable: 需要确认\n");
    printf("  NON - Non-confirmable: 不需要确认\n");
    printf("  ACK - Acknowledgement: 确认消息\n");
    printf("  RST - Reset: 复位消息\n");
}

// 演示 5: 方法码
static void demo_methods(void) {
    printf("\n=== 演示 5: 方法码 ===\n");

    typedef struct {
        const char* name;
        uint8_t code;
    } method_t;

    method_t methods[] = {
        {"GET", COAP_METHOD_GET},
        {"POST", COAP_METHOD_POST},
        {"PUT", COAP_METHOD_PUT},
        {"DELETE", COAP_METHOD_DELETE}
    };

    printf("CoAP 方法:\n\n");

    for (int i = 0; i < 4; i++) {
        printf("  %s: %d (0x%02X)\n",
               methods[i].name, methods[i].code, methods[i].code);
    }

    printf("\n响应码:\n\n");

    typedef struct {
        const char* name;
        uint8_t code;
    } response_t;

    response_t responses[] = {
        {"2.00 OK", COAP_CODE_200_OK},
        {"2.01 Created", COAP_CODE_201_CREATED},
        {"2.04 Changed", COAP_CODE_204_CHANGED},
        {"2.05 Content", COAP_CODE_205_CONTENT},
        {"4.00 Bad Request", COAP_CODE_400_BAD_REQUEST},
        {"4.04 Not Found", COAP_CODE_404_NOT_FOUND},
        {"5.00 Internal Error", COAP_CODE_500_INTERNAL_SERVER_ERROR}
    };

    for (int i = 0; i < 7; i++) {
        printf("  %s: %d (0x%02X)\n",
               responses[i].name, responses[i].code, responses[i].code);
    }
}

// 演示 6: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("CoAP 错误码:\n\n");

    coap_error_t errors[] = {
        COAP_OK,
        COAP_ERROR_INVALID_HEADER,
        COAP_ERROR_INVALID_OPTION,
        COAP_ERROR_INVALID_LENGTH,
        COAP_ERROR_MEMORY_ALLOC,
        COAP_ERROR_INVALID_PARAM
    };

    const char* error_names[] = {
        "COAP_OK",
        "COAP_ERROR_INVALID_HEADER",
        "COAP_ERROR_INVALID_OPTION",
        "COAP_ERROR_INVALID_LENGTH",
        "COAP_ERROR_MEMORY_ALLOC",
        "COAP_ERROR_INVALID_PARAM"
    };

    for (int i = 0; i < 6; i++) {
        printf("  %s (%d): %s\n",
               error_names[i], errors[i], coap_strerror(errors[i]));
    }

    printf("\n测试无效数据:\n");

    // 测试无效版本
    uint8_t invalid_ver[] = {0x00, 0x01, 0x00, 0x00};
    coap_pkt_t* pkt = coap_pkt_create();
    coap_error_t err = coap_pkt_decode(invalid_ver, sizeof(invalid_ver), pkt);
    printf("  无效版本: %s\n", coap_strerror(err));
    coap_pkt_free(pkt);
}

// 演示 7: 负载操作
static void demo_payload(void) {
    printf("\n=== 演示 7: 负载操作 ===\n");

    coap_pkt_t* pkt = coap_pkt_create();
    if (!pkt) return;

    // 设置 JSON 负载
    const char* json = "{\"temperature\":25.5,\"humidity\":60}";
    coap_pkt_set_payload(pkt, (const uint8_t*)json, strlen(json));

    printf("设置负载:\n");
    printf("  内容: %s\n", json);
    printf("  长度: %zu bytes\n", strlen(json));

    // 编码
    uint8_t buffer[256];
    size_t written;

    pkt->ver = 1;
    pkt->type = COAP_TYPE_CON;
    pkt->code = COAP_CODE_205_CONTENT;
    pkt->msg_id = 0xABCD;
    pkt->tkl = 0;

    coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);
    coap_pkt_free(pkt);

    print_hex("编码数据", buffer, written);

    // 解码验证
    pkt = coap_pkt_create();
    coap_pkt_decode(buffer, written, pkt);

    if (pkt->payload_len > 0) {
        printf("\n解码后的负载:\n");
        printf("  长度: %zu bytes\n", pkt->payload_len);
        printf("  内容: ");
        for (size_t i = 0; i < pkt->payload_len; i++) {
            printf("%c", pkt->payload[i]);
        }
        printf("\n");
    }

    coap_pkt_free(pkt);
}

// 演示 8: 观察模式
static void demo_observe(void) {
    printf("\n=== 演示 8: 观察模式 (Observe) ===\n");

    printf("CoAP 观察模式用于订阅资源变化:\n\n");

    // 订阅请求
    coap_pkt_t* pkt = coap_pkt_create();
    if (!pkt) return;

    pkt->ver = 1;
    pkt->type = COAP_TYPE_CON;
    pkt->code = COAP_METHOD_GET;
    pkt->msg_id = 0x2001;
    pkt->tkl = 0;

    // 添加 Observe 选项 (值为 0 表示注册)
    uint8_t observe_val = 0;
    coap_pkt_add_option(pkt, COAP_OPTION_OBSERVE, &observe_val, 1);

    const char* path = "temperature";
    coap_pkt_add_option(pkt, COAP_OPTION_URI_PATH, (const uint8_t*)path, strlen(path));

    printf("订阅请求:\n");
    printf("  方法: GET\n");
    printf("  路径: %s\n", path);
    printf("  Observe: 0 (注册)\n");

    uint8_t buffer[128];
    size_t written;
    coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);
    print_hex("  编码", buffer, written);
    coap_pkt_free(pkt);

    // 通知响应
    pkt = coap_pkt_create();
    if (!pkt) return;

    pkt->ver = 1;
    pkt->type = COAP_TYPE_NON;
    pkt->code = COAP_CODE_205_CONTENT;
    pkt->msg_id = 0x2002;
    pkt->tkl = 0;

    // Observe 序列号
    uint8_t seq = 1;
    coap_pkt_add_option(pkt, COAP_OPTION_OBSERVE, &seq, 1);

    const char* payload = "25.5";
    coap_pkt_set_payload(pkt, (const uint8_t*)payload, strlen(payload));

    printf("\n通知响应:\n");
    printf("  序列号: %d\n", seq);
    printf("  负载: %s°C\n", payload);

    coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);
    print_hex("  编码", buffer, written);
    coap_pkt_free(pkt);
}

// 演示 9: 块传输
static void demo_block_transfer(void) {
    printf("\n=== 演示 9: 块传输 (Block) ===\n");

    printf("CoAP 块传输用于传输大负载:\n\n");

    // Block2 选项用于请求/响应大块数据
    coap_pkt_t* pkt = coap_pkt_create();
    if (!pkt) return;

    pkt->ver = 1;
    pkt->type = COAP_TYPE_CON;
    pkt->code = COAP_METHOD_GET;
    pkt->msg_id = 0x3001;
    pkt->tkl = 0;

    // Block2 选项: NUM=0, M=0, SZX=2 (64 bytes)
    uint8_t block_val = 0x02;  // NUM=0, M=0, SZX=2
    coap_pkt_add_option(pkt, COAP_OPTION_BLOCK2, &block_val, 1);

    const char* path = "firmware";
    coap_pkt_add_option(pkt, COAP_OPTION_URI_PATH, (const uint8_t*)path, strlen(path));

    printf("块传输请求:\n");
    printf("  路径: %s\n", path);
    printf("  Block2: NUM=0, M=0, SZX=2 (64 bytes/block)\n");

    uint8_t buffer[128];
    size_t written;
    coap_pkt_encode(pkt, buffer, sizeof(buffer), &written);
    print_hex("  编码", buffer, written);
    coap_pkt_free(pkt);
}

// 演示 10: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 10: 应用场景 ===\n");

    printf("1. 物联网传感器:\n");
    printf("   - 温度/湿度读取\n");
    printf("   - 状态监控\n");
    printf("   - 远程控制\n\n");

    printf("2. 智能家居:\n");
    printf("   - 灯光控制\n");
    printf("   - 门锁管理\n");
    printf("   - 场景联动\n\n");

    printf("3. 工业监控:\n");
    printf("   - 设备状态\n");
    printf("   - 告警通知\n");
    printf("   - 数据采集\n\n");

    printf("4. 特点:\n");
    printf("   - 基于 UDP，开销小\n");
    printf("   - 支持观察模式\n");
    printf("   - 支持块传输\n");
    printf("   - 适合受限设备\n");
}

int main(void) {
    printf("========================================\n");
    printf("    CoAP 协议数据包演示程序\n");
    printf("========================================\n");

    demo_create_encode();
    demo_decode();
    demo_options();
    demo_message_types();
    demo_methods();
    demo_error_handling();
    demo_payload();
    demo_observe();
    demo_block_transfer();
    demo_use_cases();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
