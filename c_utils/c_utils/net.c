#include "net.h"
#include <stdio.h>
#include <string.h>

#ifndef _WIN32
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

bool net_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
#else
    return true;
#endif
}

void net_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

socket_t net_listen(const char *port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &res) != 0) return INVALID_SOCKET;

    socket_t fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == INVALID_SOCKET) {
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    if (bind(fd, res->ai_addr, res->ai_addrlen) != 0) {
        net_close(fd);
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }

    listen(fd, 10);
    freeaddrinfo(res);
    return fd;
}

socket_t net_connect(const char *host, const char *port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &res) != 0) return INVALID_SOCKET;

    socket_t fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == INVALID_SOCKET) {
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }

    if (connect(fd, res->ai_addr, res->ai_addrlen) != 0) {
        net_close(fd);
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }

    freeaddrinfo(res);
    return fd;
}

int net_send(socket_t fd, const void *data, size_t len) {
    return send(fd, (const char*)data, (int)len, 0);
}

int net_recv(socket_t fd, void *buf, size_t len) {
    return recv(fd, (char*)buf, (int)len, 0);
}

void net_close(socket_t fd) {
#ifdef _WIN32
    closesocket(fd);
#else
    close(fd);
#endif
}
