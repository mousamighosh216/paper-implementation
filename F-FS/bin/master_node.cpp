#include "../services/master/master_logic.h"
#include "../storage/oplog/log_manager.h"
#include "../core/network/socket.h"
#include "../core/network/connection.h"
#include <iostream>
#include <thread>

int main() {
    Socket net_init; // Initialize Winsock/POSIX
    LogManager oplog("gfs_master.log");
    MasterLogic master(oplog);
    
    std::cout << "[Master] Recovering state..." << std::endl;
    master.recover();

    socket_t server_sock = Socket::create_tcp();
    if (!Socket::bind_and_listen(server_sock, 5000)) {
        std::cerr << "Failed to bind Master to port 5000" << std::endl;
        return 1;
    }

    std::cout << "[Master] Listening on port 5000..." << std::endl;

    while (true) {
        // Simple synchronous accept for now; in Phase 13 we'd move this to a thread pool
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        socket_t client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);

        if (client_sock != INVALID_SOCK) {
            std::thread([client_sock, &master]() {
                Connection conn(client_sock);
                while (auto frame = conn.receiveFrame()) {
                    // Dispatcher Logic here (from Phase 7)
                    std::cout << "[Master] Received Request Type: " << (int)frame->type << std::endl;
                }
            }).detach();
        }
    }
    return 0;
}