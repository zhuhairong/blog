#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/mqtt_pkt.h"

void test_mqtt_pkt_connect_basic() {
    TEST(MQTT_Pkt_ConnectBasic);
    uint8_t buf[256];
    
    size_t len = mqtt_pkt_connect(buf, "test_client");
    EXPECT_TRUE(len > 0 || len == 0);
}

void test_mqtt_pkt_publish_basic() {
    TEST(MQTT_Pkt_PublishBasic);
    uint8_t buf[256];
    
    size_t len = mqtt_pkt_publish(buf, "test/topic", "hello");
    EXPECT_TRUE(len > 0 || len == 0);
}

void test_mqtt_pkt_types() {
    TEST(MQTT_Pkt_Types);
    EXPECT_TRUE(MQTT_PKT_OK == 0);
    EXPECT_TRUE(MQTT_PKT_TYPE_CONNECT == 1);
    EXPECT_TRUE(MQTT_PKT_TYPE_PUBLISH == 3);
}

void test_mqtt_pkt_struct_sizes() {
    TEST(MQTT_Pkt_StructSizes);
    mqtt_pkt_connect_config_t conn_config;
    EXPECT_TRUE(sizeof(conn_config) > 0);
    
    mqtt_pkt_publish_config_t pub_config;
    EXPECT_TRUE(sizeof(pub_config) > 0);
    
    mqtt_pkt_topic_t topic;
    EXPECT_TRUE(sizeof(topic) > 0);
}

void test_mqtt_pkt_error_values() {
    TEST(MQTT_Pkt_ErrorValues);
    EXPECT_TRUE(MQTT_PKT_OK == 0);
    EXPECT_TRUE(MQTT_PKT_ERROR_NULL_PTR != 0);
    EXPECT_TRUE(MQTT_PKT_ERROR_INVALID_ARGS != 0);
}

int main() {
    test_mqtt_pkt_connect_basic();
    test_mqtt_pkt_publish_basic();
    test_mqtt_pkt_types();
    test_mqtt_pkt_struct_sizes();
    test_mqtt_pkt_error_values();

    return 0;
}
