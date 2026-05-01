#include "tcp_connection.h"
#include "../protocol/frame.h"
#include <cstring>

TcpConnection::TcpConnection(SOCKET socket_fd) : sock(socket_fd) {}

TcpConnection::~TcpConnection() {
    if (sock != INVALID_SOCKET) {
        CLOSE_SOCKET(sock);
    }
}

bool TcpConnection::send_frame(uint8_t msg_type, const std::vector<uint8_t>& payload) {
    auto buffer = protocol::encode_frame(msg_type, payload);
    size_t total_sent = 0;
    
    while (total_sent < buffer.size()) {
        // Fix: Cast to const char* and cast size to int for Winsock
        int sent = ::send(sock, reinterpret_cast<const char*>(buffer.data() + total_sent), 
                         static_cast<int>(buffer.size() - total_sent), 0);
        
        if (sent <= 0) return false;
        total_sent += sent;
    }
    return true;
}

bool TcpConnection::read_from_socket() {
    uint8_t temp[4096];
    // Fix: Cast to char* for Winsock
    int bytes = ::recv(sock, reinterpret_cast<char*>(temp), sizeof(temp), 0);
    
    if (bytes <= 0) return false;
    read_buffer.insert(read_buffer.end(), temp, temp + bytes);
    return true;
}

bool TcpConnection::receive_frame(uint8_t& msg_type, std::vector<uint8_t>& payload) {
    while (true) {

        // need at least header
        if (read_buffer.size() >= protocol::HEADER_SIZE) {

            // check length field
            uint32_t length =
                (read_buffer[4] << 24) |
                (read_buffer[5] << 16) |
                (read_buffer[6] << 8)  |
                 read_buffer[7];

            size_t total_size = protocol::HEADER_SIZE + length;

            // full frame available?
            if (read_buffer.size() >= total_size) {

                std::vector<uint8_t> frame_bytes(
                    read_buffer.begin(),
                    read_buffer.begin() + total_size
                );

                protocol::Frame frame;
                if (!protocol::decode_frame(frame_bytes, frame)) {
                    return false;
                }

                msg_type = frame.type;
                payload = std::move(frame.payload);

                // remove consumed bytes
                read_buffer.erase(
                    read_buffer.begin(),
                    read_buffer.begin() + total_size
                );

                return true;
            }
        }

        // not enough data → read more
        if (!read_from_socket()) return false;
    }
}