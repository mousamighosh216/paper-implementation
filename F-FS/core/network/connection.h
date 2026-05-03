#pragma once
#include "socket.h"
#include "../protocol/frame.h"
#include <optional>

class Connection {
public:
    explicit Connection(socket_t fd) : sock_fd(fd) {}
    
    // High-level: Send a frame without worrying about bytes
    bool sendFrame(const protocol::Frame& frame);
    
    // High-level: Receive a full frame or return nullopt on failure
    std::optional<protocol::Frame> receiveFrame();

    void disconnect() { Socket::close_sock(sock_fd); }

private:
    socket_t sock_fd;
};