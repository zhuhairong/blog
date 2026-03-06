#include "net.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <io.h>
    #define CLOSE_SOCKET closesocket
    #define GET_SOCKET_ERROR() WSAGetLastError()
    #define IS_WOULD_BLOCK_ERROR() (WSAGetLastError() == WSAEWOULDBLOCK)
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <sys/ioctl.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #define CLOSE_SOCKET close
    #define GET_SOCKET_ERROR() errno
    #define IS_WOULD_BLOCK_ERROR() (errno == EAGAIN || errno == EWOULDBLOCK)
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

net_config_t net_default_config(void) {
    net_config_t config;
    memset(&config, 0, sizeof(config));
    config.non_blocking = false;
    config.backlog = 10;
    config.send_timeout_ms = 0;
    config.recv_timeout_ms = 0;
    config.reuse_addr = true;
    config.reuse_port = false;
    config.buffer_size = 8192;
    return config;
}

const char* net_error_string(net_error_t error) {
    switch (error) {
        case NET_OK:                  return "Success";
        case NET_ERROR_NULL_PTR:      return "Null pointer error";
        case NET_ERROR_INIT:          return "Initialization failed";
        case NET_ERROR_SOCKET_CREATE: return "Socket creation failed";
        case NET_ERROR_BIND:          return "Bind failed";
        case NET_ERROR_LISTEN:        return "Listen failed";
        case NET_ERROR_ACCEPT:        return "Accept failed";
        case NET_ERROR_CONNECT:       return "Connect failed";
        case NET_ERROR_SEND:          return "Send failed";
        case NET_ERROR_RECV:          return "Receive failed";
        case NET_ERROR_CLOSE:         return "Close failed";
        case NET_ERROR_GETADDRINFO:   return "Get address info failed";
        case NET_ERROR_INVALID_ARGS:  return "Invalid arguments";
        case NET_ERROR_TIMEOUT:       return "Timeout";
        case NET_ERROR_MAX:
        default:                      return "Unknown error";
    }
}

bool net_init(void) {
    return net_init_ex(NULL);
}

bool net_init_ex(net_error_t *error) {
#ifdef _WIN32
    WSADATA wsa;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (result != 0) {
        if (error) *error = NET_ERROR_INIT;
        return false;
    }
#else
    (void)error;
#endif
    if (error) *error = NET_OK;
    return true;
}

void net_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

bool net_set_non_blocking(socket_t fd, bool non_blocking, net_error_t *error) {
    if (fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return false;
    }

#ifdef _WIN32
    u_long mode = non_blocking ? 1 : 0;
    int result = ioctlsocket(fd, FIONBIO, &mode);
    if (result == SOCKET_ERROR) {
        if (error) *error = NET_ERROR_SOCKET_CREATE;
        return false;
    }
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        if (error) *error = NET_ERROR_SOCKET_CREATE;
        return false;
    }
    if (non_blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    if (fcntl(fd, F_SETFL, flags) == -1) {
        if (error) *error = NET_ERROR_SOCKET_CREATE;
        return false;
    }
#endif
    if (error) *error = NET_OK;
    return true;
}

bool net_set_timeout(socket_t fd, int send_timeout_ms, int recv_timeout_ms, net_error_t *error) {
    if (fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return false;
    }

    bool success = true;

    if (send_timeout_ms > 0) {
        struct timeval tv;
        tv.tv_sec = send_timeout_ms / 1000;
        tv.tv_usec = (send_timeout_ms % 1000) * 1000;
        if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv)) != 0) {
            success = false;
        }
    }

    if (recv_timeout_ms > 0) {
        struct timeval tv;
        tv.tv_sec = recv_timeout_ms / 1000;
        tv.tv_usec = (recv_timeout_ms % 1000) * 1000;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) != 0) {
            success = false;
        }
    }

    if (error) {
        *error = success ? NET_OK : NET_ERROR_SOCKET_CREATE;
    }
    return success;
}

static void set_socket_options(socket_t fd, const net_config_t *config) {
    if (!config) return;

    if (config->reuse_addr) {
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    }

#ifdef SO_REUSEPORT
    if (config->reuse_port) {
        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&opt, sizeof(opt));
    }
#else
    (void)config->reuse_port;
#endif

    if (config->send_timeout_ms > 0) {
        struct timeval tv;
        tv.tv_sec = config->send_timeout_ms / 1000;
        tv.tv_usec = (config->send_timeout_ms % 1000) * 1000;
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
    }

    if (config->recv_timeout_ms > 0) {
        struct timeval tv;
        tv.tv_sec = config->recv_timeout_ms / 1000;
        tv.tv_usec = (config->recv_timeout_ms % 1000) * 1000;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    }
}

socket_t net_listen(const char *port) {
    return net_listen_ex(port, NULL, NULL);
}

socket_t net_listen_ex(const char *port, const net_config_t *config, net_error_t *error) {
    if (!port) {
        if (error) *error = NET_ERROR_NULL_PTR;
        return INVALID_SOCKET;
    }

    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;

    int gai_result = getaddrinfo(NULL, port, &hints, &res);
    if (gai_result != 0) {
        hints.ai_family = AF_INET;
        hints.ai_flags = AI_PASSIVE;
        gai_result = getaddrinfo(NULL, port, &hints, &res);
        if (gai_result != 0) {
            if (error) *error = NET_ERROR_GETADDRINFO;
            return INVALID_SOCKET;
        }
    }

    socket_t fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == INVALID_SOCKET) {
        freeaddrinfo(res);
        if (error) *error = NET_ERROR_SOCKET_CREATE;
        return INVALID_SOCKET;
    }

    net_config_t default_cfg;
    if (!config) {
        default_cfg = net_default_config();
        config = &default_cfg;
    }

    set_socket_options(fd, config);

    if (bind(fd, res->ai_addr, (socklen_t)res->ai_addrlen) != 0) {
        CLOSE_SOCKET(fd);
        freeaddrinfo(res);
        if (error) *error = NET_ERROR_BIND;
        return INVALID_SOCKET;
    }

    int backlog = config->backlog > 0 ? config->backlog : 10;
    if (listen(fd, backlog) != 0) {
        CLOSE_SOCKET(fd);
        freeaddrinfo(res);
        if (error) *error = NET_ERROR_LISTEN;
        return INVALID_SOCKET;
    }

    if (config->non_blocking) {
        net_set_non_blocking(fd, true, NULL);
    }

    freeaddrinfo(res);
    if (error) *error = NET_OK;
    return fd;
}

socket_t net_accept(socket_t server_fd) {
    return net_accept_ex(server_fd, NULL, NULL);
}

socket_t net_accept_ex(socket_t server_fd, net_addr_t *client_addr, net_error_t *error) {
    if (server_fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return INVALID_SOCKET;
    }

    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    socket_t client_fd = accept(server_fd, (struct sockaddr*)&addr, &addr_len);
    if (client_fd == INVALID_SOCKET) {
        if (error) {
#ifdef _WIN32
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK) {
                *error = NET_ERROR_TIMEOUT;
            } else {
                *error = NET_ERROR_ACCEPT;
            }
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                *error = NET_ERROR_TIMEOUT;
            } else {
                *error = NET_ERROR_ACCEPT;
            }
#endif
        }
        return INVALID_SOCKET;
    }

    if (client_addr) {
        uint16_t port = 0;
        net_addr_to_string(&addr, client_addr->ip, sizeof(client_addr->ip), &port, NULL);
        snprintf(client_addr->port, sizeof(client_addr->port), "%u", port);
        client_addr->family = addr.ss_family;
    }

    if (error) *error = NET_OK;
    return client_fd;
}

socket_t net_connect(const char *host, const char *port) {
    return net_connect_ex(host, port, NULL, NULL);
}

socket_t net_connect_ex(const char *host, const char *port, const net_config_t *config, net_error_t *error) {
    if (!host || !port) {
        if (error) *error = NET_ERROR_NULL_PTR;
        return INVALID_SOCKET;
    }

    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int gai_result = getaddrinfo(host, port, &hints, &res);
    if (gai_result != 0) {
        if (error) *error = NET_ERROR_GETADDRINFO;
        return INVALID_SOCKET;
    }

    socket_t fd = INVALID_SOCKET;
    struct addrinfo *rp;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == INVALID_SOCKET) {
            continue;
        }

        net_config_t default_cfg;
        if (!config) {
            default_cfg = net_default_config();
            config = &default_cfg;
        }

        set_socket_options(fd, config);

        if (connect(fd, rp->ai_addr, (socklen_t)rp->ai_addrlen) == 0) {
            break;
        }

        CLOSE_SOCKET(fd);
        fd = INVALID_SOCKET;
    }

    freeaddrinfo(res);

    if (fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_CONNECT;
        return INVALID_SOCKET;
    }

    if (config->non_blocking) {
        net_set_non_blocking(fd, true, NULL);
    }

    if (error) *error = NET_OK;
    return fd;
}

int net_send(socket_t fd, const void *data, size_t len) {
    return net_send_ex(fd, data, len, NULL);
}

int net_send_ex(socket_t fd, const void *data, size_t len, net_error_t *error) {
    if (fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return SOCKET_ERROR;
    }

    if (!data) {
        if (error) *error = NET_ERROR_NULL_PTR;
        return SOCKET_ERROR;
    }

    if (len == 0) {
        if (error) *error = NET_OK;
        return 0;
    }

    if (len > INT_MAX) {
        len = INT_MAX;
    }

    int result = send(fd, (const char*)data, (int)len, 0);
    if (result == SOCKET_ERROR) {
        if (error) {
            if (IS_WOULD_BLOCK_ERROR()) {
                *error = NET_ERROR_TIMEOUT;
            } else {
                *error = NET_ERROR_SEND;
            }
        }
        return SOCKET_ERROR;
    }

    if (error) *error = NET_OK;
    return result;
}

int net_recv(socket_t fd, void *buf, size_t len) {
    return net_recv_ex(fd, buf, len, NULL);
}

int net_recv_ex(socket_t fd, void *buf, size_t len, net_error_t *error) {
    if (fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return SOCKET_ERROR;
    }

    if (!buf) {
        if (error) *error = NET_ERROR_NULL_PTR;
        return SOCKET_ERROR;
    }

    if (len == 0) {
        if (error) *error = NET_OK;
        return 0;
    }

    if (len > INT_MAX) {
        len = INT_MAX;
    }

    int result = recv(fd, (char*)buf, (int)len, 0);
    if (result == SOCKET_ERROR) {
        if (error) {
            if (IS_WOULD_BLOCK_ERROR()) {
                *error = NET_ERROR_TIMEOUT;
            } else {
                *error = NET_ERROR_RECV;
            }
        }
        return SOCKET_ERROR;
    }

    if (error) *error = NET_OK;
    return result;
}

void net_close(socket_t fd) {
    net_close_ex(fd, NULL);
}

bool net_close_ex(socket_t fd, net_error_t *error) {
    if (fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return false;
    }

    int result = CLOSE_SOCKET(fd);
    if (result != 0) {
        if (error) *error = NET_ERROR_CLOSE;
        return false;
    }

    if (error) *error = NET_OK;
    return true;
}

bool net_get_addrinfo(socket_t fd, net_addr_t *local, net_addr_t *peer, net_error_t *error) {
    if (fd == INVALID_SOCKET) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return false;
    }

    if (local) {
        struct sockaddr_storage addr;
        socklen_t addr_len = sizeof(addr);
        if (getsockname(fd, (struct sockaddr*)&addr, &addr_len) == 0) {
            uint16_t port = 0;
            net_addr_to_string(&addr, local->ip, sizeof(local->ip), &port, NULL);
            snprintf(local->port, sizeof(local->port), "%u", port);
            local->family = addr.ss_family;
        } else {
            memset(local, 0, sizeof(*local));
        }
    }

    if (peer) {
        struct sockaddr_storage addr;
        socklen_t addr_len = sizeof(addr);
        if (getpeername(fd, (struct sockaddr*)&addr, &addr_len) == 0) {
            uint16_t port = 0;
            net_addr_to_string(&addr, peer->ip, sizeof(peer->ip), &port, NULL);
            snprintf(peer->port, sizeof(peer->port), "%u", port);
            peer->family = addr.ss_family;
        } else {
            memset(peer, 0, sizeof(*peer));
        }
    }

    if (error) *error = NET_OK;
    return true;
}

bool net_get_socket_state(socket_t fd, net_socket_state_t *state, net_error_t *error) {
    if (state == NULL) {
        if (error) *error = NET_ERROR_NULL_PTR;
        return false;
    }

    memset(state, 0, sizeof(*state));

    if (fd == INVALID_SOCKET) {
        state->is_valid = false;
        if (error) *error = NET_OK;
        return true;
    }

    state->is_valid = true;

    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(fd, (struct sockaddr*)&addr, &addr_len) == 0) {
        uint16_t port = 0;
        net_addr_to_string(&addr, state->local_addr.ip, sizeof(state->local_addr.ip), &port, NULL);
        snprintf(state->local_addr.port, sizeof(state->local_addr.port), "%u", port);
        state->local_addr.family = addr.ss_family;
    }

    if (getpeername(fd, (struct sockaddr*)&addr, &addr_len) == 0) {
        state->is_connected = true;
        uint16_t port = 0;
        net_addr_to_string(&addr, state->peer_addr.ip, sizeof(state->peer_addr.ip), &port, NULL);
        snprintf(state->peer_addr.port, sizeof(state->peer_addr.port), "%u", port);
        state->peer_addr.family = addr.ss_family;
    }

    int listening = 0;
    socklen_t opt_len = sizeof(listening);
#ifdef _WIN32
    char opt_val[4];
    opt_len = sizeof(opt_val);
    if (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, opt_val, &opt_len) == 0) {
        listening = opt_val[0];
    }
#else
    if (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &listening, &opt_len) == 0 && listening) {
        state->is_listening = true;
    }
#endif
    if (listening) {
        state->is_listening = true;
    }

#ifdef _WIN32
    u_long mode = 0;
    if (ioctlsocket(fd, FIONBIO, &mode) == 0) {
        state->is_non_blocking = (mode != 0);
    }
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags != -1) {
        state->is_non_blocking = (flags & O_NONBLOCK) != 0;
    }
#endif

    if (error) *error = NET_OK;
    return true;
}

bool net_parse_ip(const char *ip_str, struct sockaddr_storage *addr, net_error_t *error) {
    if (!ip_str || !addr) {
        if (error) *error = NET_ERROR_NULL_PTR;
        return false;
    }

    memset(addr, 0, sizeof(*addr));

    struct sockaddr_in *addr4 = (struct sockaddr_in*)addr;
    if (inet_pton(AF_INET, ip_str, &addr4->sin_addr) == 1) {
        addr4->sin_family = AF_INET;
        if (error) *error = NET_OK;
        return true;
    }

    struct sockaddr_in6 *addr6 = (struct sockaddr_in6*)addr;
    if (inet_pton(AF_INET6, ip_str, &addr6->sin6_addr) == 1) {
        addr6->sin6_family = AF_INET6;
        if (error) *error = NET_OK;
        return true;
    }

    if (error) *error = NET_ERROR_INVALID_ARGS;
    return false;
}

bool net_addr_to_string(const struct sockaddr_storage *addr, char *ip_str, size_t ip_str_len, uint16_t *port, net_error_t *error) {
    if (!addr || !ip_str || ip_str_len == 0) {
        if (error) *error = NET_ERROR_NULL_PTR;
        return false;
    }

    const void *src = NULL;
    int family = addr->ss_family;

    if (family == AF_INET) {
        const struct sockaddr_in *addr4 = (const struct sockaddr_in*)addr;
        src = &addr4->sin_addr;
        if (port) *port = ntohs(addr4->sin_port);
    } else if (family == AF_INET6) {
        const struct sockaddr_in6 *addr6 = (const struct sockaddr_in6*)addr;
        src = &addr6->sin6_addr;
        if (port) *port = ntohs(addr6->sin6_port);
    } else {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return false;
    }

    if (inet_ntop(family, src, ip_str, (socklen_t)ip_str_len) == NULL) {
        if (error) *error = NET_ERROR_INVALID_ARGS;
        return false;
    }

    if (error) *error = NET_OK;
    return true;
}
