#pragma once

#include <string>
#include <vector>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    #define INVALID_SOCK INVALID_SOCKET
    #define SOCK_ERR SOCKET_ERROR
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
    typedef int socket_t;
    #define INVALID_SOCK -1
    #define SOCK_ERR -1
#endif

class Socket {
public:
    Socket(); // Initializes Winsock if on Windows
    ~Socket() = default;

    // Core Actions
    static socket_t create_tcp();
    static bool connect_to(socket_t sock, const std::string& ip, uint16_t port);
    static bool bind_and_listen(socket_t sock, uint16_t port);
    static void close_sock(socket_t sock);

    // Helpers to ensure all bytes are sent/received (Crucial for GFS)
    static bool send_all(socket_t sock, const uint8_t* data, size_t len);
    static bool recv_exact(socket_t sock, uint8_t* buffer, size_t len);

private:
    #ifdef _WIN32
        static bool winsock_initialized;
        static void init_winsock();
    #endif
};