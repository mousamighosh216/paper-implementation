#include "../core/network/socket.h"
#include "../core/network/connection.h"
#include "../core/messages/client_master.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Socket net;
    socket_t sock = Socket::create_tcp();
    
    if (!Socket::connect_to(sock, "127.0.0.1", 5000)) {
        std::cerr << "Master unreachable." << std::endl;
        return 1;
    }

    Connection conn(sock);
    
    if (argc > 2 && std::string(argv[1]) == "create") {
        msg::CreateFileRequest req { argv[2] };
        protocol::Frame f { 1, req.serialize() };
        conn.sendFrame(f);
        
        auto res = conn.receiveFrame();
        if (res && res->payload[0] == 1) {
            std::cout << "File created successfully." << std::endl;
        } else {
            std::cout << "Failed to create file." << std::endl;
        }
    }
    
    return 0;
}