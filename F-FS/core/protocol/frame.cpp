#include "frame.h"
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <arpa/inet.h>
#endif

namespace protocol {

std::vector<uint8_t> encode_frame(uint8_t type, const std::vector<uint8_t>& payload) {
    uint32_t len = static_cast<uint32_t>(payload.size());
    uint32_t net_len = htonl(len); // Convert to Big Endian for the wire

    std::vector<uint8_t> frame;
    frame.reserve(HEADER_SIZE + payload.size());

    // 1. Magic + Version + Type
    frame.push_back(MAGIC[0]);
    frame.push_back(MAGIC[1]);
    frame.push_back(VERSION);
    frame.push_back(type);

    // 2. Length (4 bytes)
    uint8_t len_bytes[4];
    std::memcpy(len_bytes, &net_len, 4);
    frame.insert(frame.end(), len_bytes, len_bytes + 4);

    // 3. Payload
    frame.insert(frame.end(), payload.begin(), payload.end());

    return frame;
}

bool decode_frame(const std::vector<uint8_t>& buffer, Frame& out_frame) {
    if (buffer.size() < HEADER_SIZE) return false;

    // Validate Magic
    if (buffer[0] != MAGIC[0] || buffer[1] != MAGIC[1]) return false;
    
    // Extract Type
    out_frame.type = buffer[3];

    // Extract Length
    uint32_t net_len;
    std::memcpy(&net_len, &buffer[4], 4);
    uint32_t host_len = ntohl(net_len);

    if (buffer.size() < HEADER_SIZE + host_len) return false;

    // Extract Payload
    out_frame.payload.assign(buffer.begin() + HEADER_SIZE, buffer.begin() + HEADER_SIZE + host_len);
    
    return true;
}

} // namespace protocol