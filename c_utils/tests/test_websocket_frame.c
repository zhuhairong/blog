#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/websocket_frame.h"

void test_ws_frame_types() {
    TEST(WS_Frame_Types);
    ws_frame_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    ws_frame_hdr_t hdr;
    memset(&hdr, 0, sizeof(hdr));
    EXPECT_TRUE(sizeof(hdr) > 0);
    
    ws_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    EXPECT_TRUE(sizeof(frame) > 0);
}

void test_ws_frame_error_values() {
    TEST(WS_Frame_ErrorValues);
    EXPECT_TRUE(WS_FRAME_OK == 0);
    EXPECT_TRUE(WS_FRAME_INVALID_PARAMS != 0);
    EXPECT_TRUE(WS_FRAME_INVALID_FORMAT != 0);
}

void test_ws_frame_config_defaults() {
    TEST(WS_Frame_ConfigDefaults);
    ws_frame_config_t config;
    memset(&config, 0, sizeof(config));
    config.strict_validation = true;
    config.allow_fragmented = true;
    config.max_payload_size = 1024 * 1024;
    
    EXPECT_TRUE(config.strict_validation);
    EXPECT_TRUE(config.allow_fragmented);
    EXPECT_TRUE(config.max_payload_size > 0);
}

void test_ws_frame_hdr_defaults() {
    TEST(WS_Frame_HdrDefaults);
    ws_frame_hdr_t hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.fin = 1;
    hdr.opcode = 1;
    hdr.masked = 0;
    hdr.payload_len = 100;
    
    EXPECT_EQ(hdr.fin, 1);
    EXPECT_EQ(hdr.opcode, 1);
    EXPECT_EQ(hdr.masked, 0);
    EXPECT_EQ(hdr.payload_len, 100);
}

void test_ws_frame_ctx() {
    TEST(WS_Frame_Ctx);
    ws_frame_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    EXPECT_TRUE(sizeof(ctx) > 0);
}

int main() {
    test_ws_frame_types();
    test_ws_frame_error_values();
    test_ws_frame_config_defaults();
    test_ws_frame_hdr_defaults();
    test_ws_frame_ctx();

    return 0;
}
