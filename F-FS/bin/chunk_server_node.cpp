#include "../services/chunkserver/chunk_store.h"
#include "../services/chunkserver/data_buffer.h"
#include "../services/chunkserver/heartbeat_manager.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: chunk_server <port> <data_dir>" << std::endl;
        return 1;
    }

    uint16_t port = std::stoi(argv[1]);
    std::string data_dir = argv[2];

    Socket net_init;
    ChunkStore store(data_dir);
    DataBuffer buffer;

    std::cout << "[ChunkServer] Starting on port " << port << "..." << std::endl;

    // Start Heartbeat thread to talk to Master (from Phase 13)
    HeartbeatManager hb("127.0.0.1", 5000, store);
    hb.start();

    // Main Server Loop to handle Client Data Pushes
    socket_t server_sock = Socket::create_tcp();
    Socket::bind_and_listen(server_sock, port);

    while (true) {
        socket_t client_sock = accept(server_sock, nullptr, nullptr);
        if (client_sock != INVALID_SOCK) {
            std::thread([client_sock, &store, &buffer]() {
                Connection conn(client_sock);
                // Handle Data Pushes and Commits (Phase 12)
            }).detach();
        }
    }

    hb.stop();
    return 0;
}