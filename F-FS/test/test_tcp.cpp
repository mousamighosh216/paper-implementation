#include "../core/network/tcp_connection.h"
// #include <arpa/inet.h> -> not valid on windows
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // Links the Winsock library
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif
#include <thread>
#include <iostream>

// server
void server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    int client_fd = accept(server_fd, nullptr, nullptr);
    TcpConnection conn(client_fd);

    uint8_t type;
    std::vector<uint8_t> payload;

    conn.receive_frame(type, payload);

    std::cout << "Server got type: " << int(type) << "\n";
}

// client
void client() {
    Sleep(1);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    TcpConnection conn(sock);

    std::vector<uint8_t> data = {10, 20, 30};
    conn.send_frame(5, data);
}

int main() {
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock init failed\n";
        return 1;
    }
#endif

    std::thread t1(server);
    std::thread t2(client);

    t1.join();
    t2.join();

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}