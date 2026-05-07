#pragma once
#include "../../core/network/connection.h"
#include "chunk_store.h"
#include <thread>
#include <atomic>

class HeartbeatManager {
public:
    HeartbeatManager(const std::string& master_ip, uint16_t port, ChunkStore& store)
        : master_ip(master_ip), port(port), store(store), running(false) {}

    void start() {
        running = true;
        worker = std::thread(&HeartbeatManager::loop, this);
    }

    void stop() {
        running = false;
        if (worker.joinable()) worker.join();
    }

private:
    void loop() {
        while (running) {
            socket_t sock = Socket::create_tcp();
            if (Socket::connect_to(sock, master_ip, port)) {
                Connection conn(sock);
                auto chunks = store.scanChunks();
                
                // Construct Heartbeat: [Type: 10][ChunkCount][List of Handles]
                Serializer s;
                s.write_vector(chunks);
                conn.sendFrame(10, s.buffer);
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    std::string master_ip;
    uint16_t port;
    ChunkStore& store;
    std::thread worker;
    std::atomic<bool> running;
};