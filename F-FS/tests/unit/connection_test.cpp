#include "../../core/network/connection.h"
#include "../../core/protocol/frame.h"
#include <cassert>
#include <iostream>
#include <thread>

// Helper to run a local echo server for testing
void run_echo_server(uint16_t port) {
    socket_t server_fd = Socket::create_tcp();
    Socket::bind_and_listen(server_fd, port);
    
    // Accept one connection
    socket_t client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd != INVALID_SOCK) {
        Connection conn(client_fd);
        auto frame = conn.receiveFrame();
        if (frame) {
            // Echo it back
            conn.sendFrame(*frame);
        }
    }
    Socket::close_sock(server_fd);
}

void test_connection_roundtrip() {
    uint16_t test_port = 5555;
    
    // 1. Start Echo Server in a background thread
    std::thread server_thread(run_echo_server, test_port);
    server_thread.detach();

    // Give the server a moment to start listening
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 2. Client Side
    socket_t sock = Socket::create_tcp();
    if (!Socket::connect_to(sock, "127.0.0.1", test_port)) {
        std::cerr << "❌ Failed to connect to test server" << std::endl;
        exit(1);
    }

    Connection conn(sock);

    // 3. Create a test frame
    protocol::Frame sent_frame;
    sent_frame.type = 5; // e.g., ClientToChunkServer::Read
    sent_frame.payload = { 'H', 'e', 'l', 'l', 'o', ' ', 'G', 'F', 'S' };

    // 4. Send and Receive (The Roundtrip)
    std::cout << "Sending frame..." << std::endl;
    assert(conn.sendFrame(sent_frame));

    std::cout << "Waiting for echo..." << std::endl;
    auto received_opt = conn.receiveFrame();
    
    // 5. Verify Results
    assert(received_opt.has_value());
    assert(received_opt->type == sent_frame.type);
    assert(received_opt->payload == sent_frame.payload);

    std::cout << "✅ Connection Roundtrip Test Passed!" << std::endl;
    conn.disconnect();
}

int main() {
    // Initialize Networking (for Windows)
    Socket init; 
    test_connection_roundtrip();
    return 0;
}