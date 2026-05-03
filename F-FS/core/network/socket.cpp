// this simplifies arpa ext logic as as now we can dir comm bt diff os

#include "socket.h"
#include <iostream>

#ifdef _WIN32
bool Socket::winsock_initialized = false;
void Socket::init_winsock() {
    if (!winsock_initialized) {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        winsock_initialized = true;
    }
}
#endif

Socket::Socket() {
#ifdef _WIN32
    init_winsock();
#endif
}

socket_t Socket::create_tcp() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

bool Socket::send_all(socket_t sock, const uint8_t* data, size_t len) {
    size_t total_sent = 0;
    while (total_sent < len) {
        int sent = send(sock, (const char*)(data + total_sent), (int)(len - total_sent), 0);
        if (sent <= 0) return false;
        total_sent += sent;
    }
    return true;
}

bool Socket::recv_exact(socket_t sock, uint8_t* buffer, size_t len) {
    size_t total_recvd = 0;
    while (total_recvd < len) {
        int recvd = recv(sock, (char*)(buffer + total_recvd), (int)(len - total_recvd), 0);
        if (recvd <= 0) return false;
        total_recvd += recvd;
    }
    return true;
}

void Socket::close_sock(socket_t sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

bool Socket::connect_to(socket_t sock, const std::string& ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

#ifdef _WIN32
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
#else
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
#endif

    return connect(sock, (sockaddr*)&addr, sizeof(addr)) != SOCK_ERR;
}

bool Socket::bind_and_listen(socket_t sock, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCK_ERR)
        return false;

    if (listen(sock, 10) == SOCK_ERR)
        return false;

    return true;
}