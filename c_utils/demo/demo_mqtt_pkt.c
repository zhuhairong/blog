/**
 * MQTT 报文处理演示程序
 *
 * 功能：
 * - MQTT 报文类型
 * - 连接报文构建
 * - 发布/订阅报文
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/mqtt_pkt.h"

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

// 演示 3: 连接配置
static void demo_connect_config(void) {
    printf("\n=== 演示 3: 连接配置 ===\n");

    mqtt_pkt_connect_config_t config;
    memset(&config, 0, sizeof(config));

    config.client_id = "demo_client_001";
    config.username = "user";
    config.password = "pass";
    config.clean_session = true;
    config.keep_alive = 60;
    config.protocol_level = 4;  // MQTT 3.1.1

    printf("连接配置示例:\n");
    printf("  客户端 ID: %s\n", config.client_id);
    printf("  用户名: %s\n", config.username);
    printf("  清理会话: %s\n", config.clean_session ? "是" : "否");
    printf("  保活时间: %d 秒\n", config.keep_alive);
    printf("  协议版本: MQTT %s\n", config.protocol_level == 4 ? "3.1.1" : "5.0");

    printf("\n遗嘱消息配置:\n");
    printf("  遗嘱主题: device/status\n");
    printf("  遗嘱消息: offline\n");
    printf("  遗嘱 QoS: 1\n");
    printf("  遗嘱保留: 是\n");
}

// 演示 4: 发布配置
static void demo_publish_config(void) {
    printf("\n=== 演示 4: 发布消息配置 ===\n");

    mqtt_pkt_publish_config_t config;
    memset(&config, 0, sizeof(config));

    config.topic = "home/livingroom/temperature";
    config.payload = "23.5";
    config.payload_len = strlen(config.payload);
    config.qos = 1;
    config.retain = true;
    config.packet_id = 1234;

    printf("发布配置示例:\n");
    printf("  主题: %s\n", config.topic);
    printf("  载荷: %s\n", (char*)config.payload);
    printf("  载荷长度: %zu\n", config.payload_len);
    printf("  QoS: %d\n", config.qos);
    printf("  保留: %s\n", config.retain ? "是" : "否");
    printf("  报文 ID: %d\n", config.packet_id);

    printf("\n主题层级:\n");
    printf("  home/                  - 家\n");
    printf("  home/livingroom/       - 客厅\n");
    printf("  home/livingroom/temp   - 温度\n");
    printf("  home/bedroom/light     - 卧室灯光\n");
}

// 演示 5: 订阅主题
static void demo_subscribe(void) {
    printf("\n=== 演示 5: 订阅主题 ===\n");

    printf("订阅示例:\n\n");

    mqtt_pkt_topic_t topics[] = {
        {"home/+/temperature", 1},  // + 单层通配符
        {"home/livingroom/#", 0},   // # 多层通配符
        {"device/+/status", 2},
    };

    for (int i = 0; i < 3; i++) {
        printf("  主题过滤器: %s\n", topics[i].topic_filter);
        printf("  QoS: %d\n\n", topics[i].qos);
    }

    printf("通配符说明:\n");
    printf("  + : 匹配单层 (如 home/+/temp 匹配 home/kitchen/temp)\n");
    printf("  # : 匹配多层 (如 home/# 匹配 home/any/thing)\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

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

// 演示 7: 协议特点
static void demo_features(void) {
    printf("\n=== 演示 7: MQTT 协议特点 ===\n");

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
    demo_connect_config();
    demo_publish_config();
    demo_subscribe();
    demo_applications();
    demo_features();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
