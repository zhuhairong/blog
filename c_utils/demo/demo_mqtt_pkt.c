/**
 * MQTT 报文处理演示程序
 *
 * 功能：
 * - MQTT 报文类型
 * - 连接报文构建
 * - 发布/订阅报文
 * - 报文编码和解码
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/mqtt_pkt.h"

// 打印十六进制数据
static void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len && i < 32; i++) {
        printf("%02X ", data[i]);
    }
    if (len > 32) printf("...");
    printf("\n");
}

// 演示 1: MQTT 报文类型
static void demo_packet_types(void) {
    printf("\n=== 演示 1: MQTT 报文类型 ===\n");

    printf("MQTT 协议报文类型:\n\n");

    printf("连接管理:\n");
    printf("  CONNECT     (1)  - 连接请求\n");
    printf("  CONNACK     (2)  - 连接确认\n");
    printf("  DISCONNECT  (14) - 断开连接\n\n");

    printf("消息发布:\n");
    printf("  PUBLISH     (3)  - 发布消息\n");
    printf("  PUBACK      (4)  - 发布确认 (QoS 1)\n");
    printf("  PUBREC      (5)  - 发布接收 (QoS 2)\n");
    printf("  PUBREL      (6)  - 发布释放 (QoS 2)\n");
    printf("  PUBCOMP     (7)  - 发布完成 (QoS 2)\n\n");

    printf("订阅管理:\n");
    printf("  SUBSCRIBE   (8)  - 订阅请求\n");
    printf("  SUBACK      (9)  - 订阅确认\n");
    printf("  UNSUBSCRIBE (10) - 取消订阅\n");
    printf("  UNSUBACK    (11) - 取消订阅确认\n\n");

    printf("心跳检测:\n");
    printf("  PINGREQ     (12) - Ping 请求\n");
    printf("  PINGRESP    (13) - Ping 响应\n");
}

// 演示 2: QoS 等级
static void demo_qos(void) {
    printf("\n=== 演示 2: QoS 服务质量等级 ===\n");

    printf("QoS 0 - 最多一次:\n");
    printf("  消息只发送一次\n");
    printf("  不保证送达\n");
    printf("  性能最好\n\n");

    printf("QoS 1 - 至少一次:\n");
    printf("  保证消息送达\n");
    printf("  可能重复\n");
    printf("  需要 PUBACK 确认\n\n");

    printf("QoS 2 - 恰好一次:\n");
    printf("  保证消息只送达一次\n");
    printf("  不会重复\n");
    printf("  四次握手 (PUBREC, PUBREL, PUBCOMP)\n\n");

    printf("选择建议:\n");
    printf("  - 传感器数据: QoS 0\n");
    printf("  - 命令下发: QoS 1\n");
    printf("  - 关键消息: QoS 2\n");
}

// 演示 3: 连接报文构建和编码
static void demo_connect_packet(void) {
    printf("\n=== 演示 3: 连接报文构建和编码 ===\n");

    uint8_t buffer[256];

    // 构建连接报文
    const char *client_id = "demo_client_001";
    printf("客户端 ID: %s\n", client_id);

    // 编码连接报文
    size_t size = mqtt_pkt_connect(buffer, client_id);
    if (size > 0) {
        print_hex("连接报文", buffer, size);
    } else {
        printf("编码失败\n");
    }
}

// 演示 4: 发布报文构建和编码
static void demo_publish_packet(void) {
    printf("\n=== 演示 4: 发布报文构建和编码 ===\n");

    uint8_t buffer[256];

    // 构建发布报文
    const char *topic = "home/livingroom/temperature";
    const char *payload = "23.5";

    printf("发布配置:\n");
    printf("  主题: %s\n", topic);
    printf("  载荷: %s\n", payload);

    // 编码发布报文
    size_t size = mqtt_pkt_publish(buffer, topic, payload);
    if (size > 0) {
        print_hex("发布报文", buffer, size);
    } else {
        printf("编码失败\n");
    }
}

// 演示 5: 订阅报文构建和编码
static void demo_subscribe_packet(void) {
    printf("\n=== 演示 5: 订阅报文构建和编码 ===\n");

    printf("订阅主题示例:\n");
    printf("  1. home/+/temperature (QoS 1) - 单层通配符\n");
    printf("  2. home/livingroom/# (QoS 0) - 多层通配符\n");
    printf("  3. device/+/status (QoS 2) - 设备状态\n");
    printf("\n注意：库中未实现订阅报文构建函数\n");
}

// 演示 6: 心跳报文构建和编码
static void demo_ping_packet(void) {
    printf("\n=== 演示 6: 心跳报文构建和编码 ===\n");
    printf("注意：库中未实现心跳报文构建函数\n");
    printf("Ping 请求报文格式: 0xC0 0x00\n");
    printf("Ping 响应报文格式: 0xD0 0x00\n");
}

// 演示 7: 断开连接报文构建和编码
static void demo_disconnect_packet(void) {
    printf("\n=== 演示 7: 断开连接报文构建和编码 ===\n");
    printf("注意：库中未实现断开连接报文构建函数\n");
    printf("断开连接报文格式: 0xE0 0x00\n");
}

// 演示 8: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 8: 应用场景 ===\n");

    printf("1. 物联网 (IoT)\n");
    printf("   - 传感器数据采集\n");
    printf("   - 设备远程控制\n");
    printf("   - 状态监控\n\n");

    printf("2. 智能家居\n");
    printf("   - 灯光控制\n");
    printf("   - 温度调节\n");
    printf("   - 安防系统\n\n");

    printf("3. 工业自动化\n");
    printf("   - 设备监控\n");
    printf("   - 数据采集\n");
    printf("   - 远程维护\n\n");

    printf("4. 即时通讯\n");
    printf("   - 消息推送\n");
    printf("   - 在线状态\n");
    printf("   - 通知系统\n\n");

    printf("5. 车联网\n");
    printf("   - 车辆定位\n");
    printf("   - 远程诊断\n");
    printf("   - 数据上传\n");
}

// 演示 9: 协议特点
static void demo_features(void) {
    printf("\n=== 演示 9: MQTT 协议特点 ===\n");

    printf("轻量级:\n");
    printf("  - 报文头最小 2 字节\n");
    printf("  - 低带宽消耗\n");
    printf("  - 适合嵌入式设备\n\n");

    printf("发布/订阅模式:\n");
    printf("  - 解耦生产者和消费者\n");
    printf("  - 支持多对多通信\n");
    printf("  - 灵活的消息路由\n\n");

    printf("可靠性:\n");
    printf("  - 三种 QoS 等级\n");
    printf("  - 保留消息\n");
    printf("  - 遗嘱消息\n\n");

    printf("安全性:\n");
    printf("  - TLS/SSL 加密\n");
    printf("  - 用户名/密码认证\n");
    printf("  - 访问控制列表\n");
}

int main(void) {
    printf("========================================\n");
    printf("    MQTT 报文处理演示\n");
    printf("========================================\n");

    demo_packet_types();
    demo_qos();
    demo_connect_packet();
    demo_publish_packet();
    demo_subscribe_packet();
    demo_ping_packet();
    demo_disconnect_packet();
    demo_applications();
    demo_features();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
