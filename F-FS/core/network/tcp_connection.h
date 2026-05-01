#pragma once
#include <vector>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    // Only typedef if not using Mingw64 (which usually defines it in corecrt.h)
    #ifndef _SSIZE_T_DEFINED
        #define _SSIZE_T_DEFINED
        typedef intptr_t ssize_t; 
    #endif
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define CLOSE_SOCKET close
    typedef int SOCKET;
    #define INVALID_SOCKET -1
#endif

class TcpConnection {
public:
    explicit TcpConnection(SOCKET socket_fd);
    ~TcpConnection();

    bool send_frame(uint8_t msg_type, const std::vector<uint8_t>& payload);
    bool receive_frame(uint8_t &msg_type, std::vector<uint8_t>& payload);

private:
    SOCKET sock;
    std::vector<uint8_t> read_buffer;
    bool read_from_socket();
};