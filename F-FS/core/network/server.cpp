#include "server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <iostream>

Server::Server(int port) : port(port) {
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Allow address reuse (prevents "Address already in use" errors on restart)
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    listen(listen_sock, 10);
}

void Server::run(HandlerFunc handler) {
    std::cout << "Server listening on port " << port << "..." << std::endl;
    while (true) {
        int client_fd = accept(listen_sock, nullptr, nullptr);
        if (client_fd < 0) continue;

        // Create a Connection object and move it into a new thread
        // This is the C++ equivalent of tokio::spawn(handle_connection(...))
        std::thread([client_fd, handler]() {
            Connection conn(client_fd);
            handler(std::move(conn));
        }).detach();
    }
}

// close(listen_sock)