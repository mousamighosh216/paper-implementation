#include "connection.h"
#include <cstring>

bool Connection::sendFrame(const protocol::Frame& frame) {
    std::vector<uint8_t> wire_data = protocol::encode_frame(frame.type, frame.payload);
    return Socket::send_all(sock_fd, wire_data.data(), wire_data.size());
}

std::optional<protocol::Frame> Connection::receiveFrame() {
    uint8_t header[8];
    // 1. Get the header first
    if (!Socket::recv_exact(sock_fd, header, 8)) return std::nullopt;

    // 2. Quick validation of Magic Bytes
    if (header[0] != protocol::MAGIC[0] || header[1] != protocol::MAGIC[1]) return std::nullopt;

    // 3. Extract length (Match Phase 1 decoding)
    uint32_t net_len;
    std::memcpy(&net_len, &header[4], 4);
    uint32_t payload_len = ntohl(net_len);

    // 4. Read the specific payload size
    std::vector<uint8_t> payload(payload_len);
    if (!Socket::recv_exact(sock_fd, payload.data(), payload_len)) return std::nullopt;

    return protocol::Frame{header[3], payload};
}