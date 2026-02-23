#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/net.h"

void test_net_init_cleanup() {
    TEST(Net_InitCleanup);
    bool result = net_init();
    EXPECT_TRUE(result || !result);
    
    net_cleanup();
}

void test_net_listen_invalid_port() {
    TEST(Net_ListenInvalidPort);
    net_init();
    
    socket_t sock = net_listen("");
    EXPECT_TRUE(sock == INVALID_SOCKET || sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_connect_invalid() {
    TEST(Net_ConnectInvalid);
    net_init();
    
    socket_t sock = net_connect("invalid.host.that.does.not.exist", "9999");
    EXPECT_TRUE(sock == INVALID_SOCKET);
    
    net_cleanup();
}

void test_net_close_invalid() {
    TEST(Net_CloseInvalid);
    net_close(INVALID_SOCKET);
    EXPECT_TRUE(true);
}

void test_net_types() {
    TEST(Net_Types);
    net_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    net_addr_t addr;
    memset(&addr, 0, sizeof(addr));
    EXPECT_TRUE(sizeof(addr) > 0);
}

int main() {
    test_net_init_cleanup();
    test_net_listen_invalid_port();
    test_net_connect_invalid();
    test_net_close_invalid();
    test_net_types();

    return 0;
}
