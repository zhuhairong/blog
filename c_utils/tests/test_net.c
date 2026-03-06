#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../c_utils/utest.h"
#include "../c_utils/net.h"

#define TEST_PORT_BASE 18000
#define TEST_BUFFER_SIZE 256

static int g_test_port = TEST_PORT_BASE;

static int get_unique_port(void) {
    return __sync_fetch_and_add(&g_test_port, 1);
}

void test_net_init_success(void) {
    TEST(Net_InitSuccess);
    bool result = net_init();
    EXPECT_TRUE(result);
    net_cleanup();
}

void test_net_init_cleanup_multiple(void) {
    TEST(Net_InitCleanupMultiple);
    bool result1 = net_init();
    EXPECT_TRUE(result1);
    
    bool result2 = net_init();
    EXPECT_TRUE(result2);
    
    net_cleanup();
    net_cleanup();
}

void test_net_init_ex_error(void) {
    TEST(Net_InitExError);
    net_error_t error = NET_ERROR_MAX;
    bool result = net_init_ex(&error);
    EXPECT_TRUE(result);
    EXPECT_TRUE(error == NET_OK);
    net_cleanup();
}

void test_net_default_config(void) {
    TEST(Net_DefaultConfig);
    net_config_t config = net_default_config();
    EXPECT_FALSE(config.non_blocking);
    EXPECT_TRUE(config.backlog > 0);
    EXPECT_TRUE(config.buffer_size > 0);
}

void test_net_listen_valid_port(void) {
    TEST(Net_ListenValidPort);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t sock = net_listen(port_str);
    EXPECT_TRUE(sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_listen_port_zero(void) {
    TEST(Net_ListenPortZero);
    net_init();
    
    socket_t sock = net_listen("0");
    EXPECT_TRUE(sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_listen_null_port(void) {
    TEST(Net_ListenNullPort);
    net_init();
    
    socket_t sock = net_listen(NULL);
    EXPECT_TRUE(sock == INVALID_SOCKET);
    
    net_cleanup();
}

void test_net_listen_ex_error(void) {
    TEST(Net_ListenExError);
    net_init();
    
    net_error_t error = NET_ERROR_MAX;
    socket_t sock = net_listen_ex(NULL, NULL, &error);
    EXPECT_TRUE(sock == INVALID_SOCKET);
    EXPECT_TRUE(error == NET_ERROR_NULL_PTR || error == NET_ERROR_INVALID_ARGS);
    
    net_cleanup();
}

void test_net_listen_with_config(void) {
    TEST(Net_ListenWithConfig);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    net_config_t config = net_default_config();
    config.reuse_addr = true;
    
    net_error_t error = NET_ERROR_MAX;
    socket_t sock = net_listen_ex(port_str, &config, &error);
    EXPECT_TRUE(sock != INVALID_SOCKET);
    EXPECT_TRUE(error == NET_OK);
    
    if (sock != INVALID_SOCKET) {
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_connect_localhost(void) {
    TEST(Net_ConnectLocalhost);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t server = net_listen(port_str);
    EXPECT_TRUE(server != INVALID_SOCKET);
    
    if (server != INVALID_SOCKET) {
        socket_t client = net_connect("127.0.0.1", port_str);
        EXPECT_TRUE(client != INVALID_SOCKET);
        
        if (client != INVALID_SOCKET) {
            net_close(client);
        }
        
        net_close(server);
    }
    
    net_cleanup();
}

void test_net_connect_null_host(void) {
    TEST(Net_ConnectNullHost);
    net_init();
    
    socket_t sock = net_connect(NULL, "8080");
    EXPECT_TRUE(sock == INVALID_SOCKET);
    
    net_cleanup();
}

void test_net_connect_null_port(void) {
    TEST(Net_ConnectNullPort);
    net_init();
    
    socket_t sock = net_connect("127.0.0.1", NULL);
    EXPECT_TRUE(sock == INVALID_SOCKET);
    
    net_cleanup();
}

void test_net_connect_invalid_host(void) {
    TEST(Net_ConnectInvalidHost);
    net_init();
    
    socket_t sock = net_connect("invalid.host.that.does.not.exist", "9999");
    EXPECT_TRUE(sock == INVALID_SOCKET);
    
    net_cleanup();
}

void test_net_connect_ex_error(void) {
    TEST(Net_ConnectExError);
    net_init();
    
    net_error_t error = NET_ERROR_MAX;
    socket_t sock = net_connect_ex(NULL, "8080", NULL, &error);
    EXPECT_TRUE(sock == INVALID_SOCKET);
    EXPECT_TRUE(error == NET_ERROR_NULL_PTR || error == NET_ERROR_INVALID_ARGS);
    
    net_cleanup();
}

void test_net_send_recv_basic(void) {
    TEST(Net_SendRecvBasic);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t server = net_listen(port_str);
    EXPECT_TRUE(server != INVALID_SOCKET);
    
    if (server != INVALID_SOCKET) {
        socket_t client = net_connect("127.0.0.1", port_str);
        EXPECT_TRUE(client != INVALID_SOCKET);
        
        if (client != INVALID_SOCKET) {
            socket_t accepted = net_accept(server);
            EXPECT_TRUE(accepted != INVALID_SOCKET);
            
            if (accepted != INVALID_SOCKET) {
                const char *msg = "Hello, Network!";
                int sent = net_send(client, msg, strlen(msg));
                EXPECT_TRUE(sent == (int)strlen(msg));
                
                char buf[TEST_BUFFER_SIZE] = {0};
                int received = net_recv(accepted, buf, sizeof(buf) - 1);
                EXPECT_TRUE(received == (int)strlen(msg));
                EXPECT_TRUE(strcmp(buf, msg) == 0);
                
                net_close(accepted);
            }
            
            net_close(client);
        }
        
        net_close(server);
    }
    
    net_cleanup();
}

void test_net_send_null_data(void) {
    TEST(Net_SendNullData);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t server = net_listen(port_str);
    
    if (server != INVALID_SOCKET) {
        socket_t client = net_connect("127.0.0.1", port_str);
        
        if (client != INVALID_SOCKET) {
            int sent = net_send(client, NULL, 10);
            EXPECT_TRUE(sent == SOCKET_ERROR);
            net_close(client);
        }
        
        net_close(server);
    }
    
    net_cleanup();
}

void test_net_recv_null_buffer(void) {
    TEST(Net_RecvNullBuffer);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t server = net_listen(port_str);
    
    if (server != INVALID_SOCKET) {
        socket_t client = net_connect("127.0.0.1", port_str);
        
        if (client != INVALID_SOCKET) {
            int received = net_recv(client, NULL, 10);
            EXPECT_TRUE(received == SOCKET_ERROR);
            net_close(client);
        }
        
        net_close(server);
    }
    
    net_cleanup();
}

void test_net_send_ex_error(void) {
    TEST(Net_SendExError);
    net_init();
    
    net_error_t error = NET_ERROR_MAX;
    int sent = net_send_ex(INVALID_SOCKET, "test", 4, &error);
    EXPECT_TRUE(sent == SOCKET_ERROR);
    EXPECT_TRUE(error != NET_OK);
    
    net_cleanup();
}

void test_net_recv_ex_error(void) {
    TEST(Net_RecvExError);
    net_init();
    
    char buf[10];
    net_error_t error = NET_ERROR_MAX;
    int received = net_recv_ex(INVALID_SOCKET, buf, sizeof(buf), &error);
    EXPECT_TRUE(received == SOCKET_ERROR);
    EXPECT_TRUE(error != NET_OK);
    
    net_cleanup();
}

void test_net_close_invalid(void) {
    TEST(Net_CloseInvalid);
    net_error_t error = NET_ERROR_MAX;
    bool result = net_close_ex(INVALID_SOCKET, &error);
    EXPECT_FALSE(result);
    EXPECT_TRUE(error != NET_OK);
}

void test_net_accept_invalid(void) {
    TEST(Net_AcceptInvalid);
    net_init();
    
    socket_t sock = net_accept(INVALID_SOCKET);
    EXPECT_TRUE(sock == INVALID_SOCKET);
    
    net_cleanup();
}

void test_net_accept_ex_error(void) {
    TEST(Net_AcceptExError);
    net_init();
    
    net_error_t error = NET_ERROR_MAX;
    net_addr_t client_addr;
    socket_t sock = net_accept_ex(INVALID_SOCKET, &client_addr, &error);
    EXPECT_TRUE(sock == INVALID_SOCKET);
    EXPECT_TRUE(error != NET_OK);
    
    net_cleanup();
}

void test_net_set_non_blocking(void) {
    TEST(Net_SetNonBlocking);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t sock = net_listen(port_str);
    EXPECT_TRUE(sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        net_error_t error = NET_ERROR_MAX;
        bool result = net_set_non_blocking(sock, true, &error);
        EXPECT_TRUE(result);
        EXPECT_TRUE(error == NET_OK);
        
        result = net_set_non_blocking(sock, false, &error);
        EXPECT_TRUE(result);
        
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_set_timeout(void) {
    TEST(Net_SetTimeout);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t sock = net_listen(port_str);
    EXPECT_TRUE(sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        net_error_t error = NET_ERROR_MAX;
        bool result = net_set_timeout(sock, 1000, 1000, &error);
        EXPECT_TRUE(result);
        EXPECT_TRUE(error == NET_OK);
        
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_get_addrinfo(void) {
    TEST(Net_GetAddrinfo);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t sock = net_listen(port_str);
    EXPECT_TRUE(sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        net_addr_t local, peer;
        net_error_t error = NET_ERROR_MAX;
        bool result = net_get_addrinfo(sock, &local, &peer, &error);
        EXPECT_TRUE(result);
        EXPECT_TRUE(error == NET_OK);
        EXPECT_TRUE(strlen(local.port) > 0);
        
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_get_socket_state(void) {
    TEST(Net_GetSocketState);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t sock = net_listen(port_str);
    EXPECT_TRUE(sock != INVALID_SOCKET);
    
    if (sock != INVALID_SOCKET) {
        net_socket_state_t state;
        net_error_t error = NET_ERROR_MAX;
        bool result = net_get_socket_state(sock, &state, &error);
        EXPECT_TRUE(result);
        EXPECT_TRUE(error == NET_OK);
        EXPECT_TRUE(state.is_valid);
        EXPECT_TRUE(state.is_listening);
        
        net_close(sock);
    }
    
    net_cleanup();
}

void test_net_error_string(void) {
    TEST(Net_ErrorString);
    const char *str;
    
    str = net_error_string(NET_OK);
    EXPECT_TRUE(str != NULL);
    EXPECT_TRUE(strlen(str) > 0);
    
    str = net_error_string(NET_ERROR_NULL_PTR);
    EXPECT_TRUE(str != NULL);
    EXPECT_TRUE(strlen(str) > 0);
    
    str = net_error_string(NET_ERROR_MAX);
    EXPECT_TRUE(str != NULL);
}

typedef struct {
    socket_t server;
    int port;
    int client_count;
    bool success;
} server_context_t;

static void* server_thread_func(void* arg) {
    server_context_t* ctx = (server_context_t*)arg;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", ctx->port);
    
    ctx->server = net_listen(port_str);
    if (ctx->server == INVALID_SOCKET) {
        ctx->success = false;
        return NULL;
    }
    
    ctx->success = true;
    
    for (int i = 0; i < ctx->client_count; i++) {
        socket_t client = net_accept(ctx->server);
        if (client != INVALID_SOCKET) {
            char buf[TEST_BUFFER_SIZE];
            int received = net_recv(client, buf, sizeof(buf));
            if (received > 0) {
                net_send(client, buf, received);
            }
            net_close(client);
        }
    }
    
    return NULL;
}

void test_net_client_server_communication(void) {
    TEST(Net_ClientServerCommunication);
    net_init();
    
    int port = get_unique_port();
    
    server_context_t ctx = {
        .server = INVALID_SOCKET,
        .port = port,
        .client_count = 1,
        .success = false
    };
    
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_thread_func, &ctx);
    
    usleep(100000);
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t client = net_connect("127.0.0.1", port_str);
    EXPECT_TRUE(client != INVALID_SOCKET);
    
    if (client != INVALID_SOCKET) {
        const char *msg = "Test Message";
        int sent = net_send(client, msg, strlen(msg));
        EXPECT_TRUE(sent == (int)strlen(msg));
        
        char buf[TEST_BUFFER_SIZE] = {0};
        int received = net_recv(client, buf, sizeof(buf));
        EXPECT_TRUE(received == (int)strlen(msg));
        EXPECT_TRUE(strcmp(buf, msg) == 0);
        
        net_close(client);
    }
    
    pthread_join(server_thread, NULL);
    EXPECT_TRUE(ctx.success);
    
    if (ctx.server != INVALID_SOCKET) {
        net_close(ctx.server);
    }
    
    net_cleanup();
}

typedef struct {
    int client_id;
    int port;
    bool success;
} client_context_t;

static void* client_thread_func(void* arg) {
    client_context_t* ctx = (client_context_t*)arg;
    
    usleep(50000);
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", ctx->port);
    
    socket_t sock = net_connect("127.0.0.1", port_str);
    if (sock == INVALID_SOCKET) {
        ctx->success = false;
        return NULL;
    }
    
    char msg[64];
    snprintf(msg, sizeof(msg), "Client %d", ctx->client_id);
    
    int sent = net_send(sock, msg, strlen(msg));
    if (sent != (int)strlen(msg)) {
        net_close(sock);
        ctx->success = false;
        return NULL;
    }
    
    char buf[TEST_BUFFER_SIZE] = {0};
    int received = net_recv(sock, buf, sizeof(buf));
    ctx->success = (received == (int)strlen(msg) && strcmp(buf, msg) == 0);
    
    net_close(sock);
    return NULL;
}

void test_net_concurrent_connections(void) {
    TEST(Net_ConcurrentConnections);
    net_init();
    
    int port = get_unique_port();
    const int num_clients = 3;
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t server = net_listen(port_str);
    EXPECT_TRUE(server != INVALID_SOCKET);
    
    if (server == INVALID_SOCKET) {
        net_cleanup();
        return;
    }
    
    pthread_t client_threads[num_clients];
    client_context_t client_ctxs[num_clients];
    
    for (int i = 0; i < num_clients; i++) {
        client_ctxs[i].client_id = i;
        client_ctxs[i].port = port;
        client_ctxs[i].success = false;
        pthread_create(&client_threads[i], NULL, client_thread_func, &client_ctxs[i]);
    }
    
    int handled = 0;
    while (handled < num_clients) {
        socket_t client = net_accept(server);
        if (client != INVALID_SOCKET) {
            char buf[TEST_BUFFER_SIZE];
            int received = net_recv(client, buf, sizeof(buf));
            if (received > 0) {
                net_send(client, buf, received);
            }
            net_close(client);
            handled++;
        }
    }
    
    for (int i = 0; i < num_clients; i++) {
        pthread_join(client_threads[i], NULL);
        EXPECT_TRUE(client_ctxs[i].success);
    }
    
    net_close(server);
    net_cleanup();
}

void test_net_types_size(void) {
    TEST(Net_TypesSize);
    EXPECT_TRUE(sizeof(net_error_t) > 0);
    EXPECT_TRUE(sizeof(net_config_t) > 0);
    EXPECT_TRUE(sizeof(net_addr_t) > 0);
    EXPECT_TRUE(sizeof(net_socket_state_t) > 0);
}

void test_net_large_data_transfer(void) {
    TEST(Net_LargeDataTransfer);
    net_init();
    
    int port = get_unique_port();
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    socket_t server = net_listen(port_str);
    EXPECT_TRUE(server != INVALID_SOCKET);
    
    if (server != INVALID_SOCKET) {
        socket_t client = net_connect("127.0.0.1", port_str);
        EXPECT_TRUE(client != INVALID_SOCKET);
        
        if (client != INVALID_SOCKET) {
            socket_t accepted = net_accept(server);
            EXPECT_TRUE(accepted != INVALID_SOCKET);
            
            if (accepted != INVALID_SOCKET) {
                const int data_size = 4096;
                char *send_data = (char*)malloc(data_size);
                char *recv_data = (char*)malloc(data_size);
                
                for (int i = 0; i < data_size; i++) {
                    send_data[i] = (char)(i % 256);
                }
                memset(recv_data, 0, data_size);
                
                int total_sent = 0;
                while (total_sent < data_size) {
                    int sent = net_send(client, send_data + total_sent, data_size - total_sent);
                    if (sent <= 0) break;
                    total_sent += sent;
                }
                EXPECT_TRUE(total_sent == data_size);
                
                int total_received = 0;
                while (total_received < data_size) {
                    int received = net_recv(accepted, recv_data + total_received, data_size - total_received);
                    if (received <= 0) break;
                    total_received += received;
                }
                EXPECT_TRUE(total_received == data_size);
                EXPECT_TRUE(memcmp(send_data, recv_data, data_size) == 0);
                
                free(send_data);
                free(recv_data);
                net_close(accepted);
            }
            net_close(client);
        }
        net_close(server);
    }
    
    net_cleanup();
}

int main(void) {
    test_net_init_success();
    test_net_init_cleanup_multiple();
    test_net_init_ex_error();
    test_net_default_config();
    
    test_net_listen_valid_port();
    test_net_listen_port_zero();
    test_net_listen_null_port();
    test_net_listen_ex_error();
    test_net_listen_with_config();
    
    test_net_connect_localhost();
    test_net_connect_null_host();
    test_net_connect_null_port();
    test_net_connect_invalid_host();
    test_net_connect_ex_error();
    
    test_net_send_recv_basic();
    test_net_send_null_data();
    test_net_recv_null_buffer();
    test_net_send_ex_error();
    test_net_recv_ex_error();
    
    test_net_close_invalid();
    test_net_accept_invalid();
    test_net_accept_ex_error();
    
    test_net_set_non_blocking();
    test_net_set_timeout();
    test_net_get_addrinfo();
    test_net_get_socket_state();
    test_net_error_string();
    
    test_net_client_server_communication();
    test_net_concurrent_connections();
    
    test_net_types_size();
    test_net_large_data_transfer();
    
    return 0;
}
