// Responsibilities
// Define binary framing format
// Serialize/deserialize raw payload (NOT business objects)

#pragma once
#include <cstdint>
#include <vector>

namespace protocol {

constexpr uint8_t MAGIC[2] = {0x4A, 0x46}; // 'J' 'F'
constexpr uint8_t VERSION = 1;
constexpr size_t HEADER_SIZE = 8;
constexpr size_t MAX_FRAME_SIZE = 64 * 1024 * 1024 + 1024;

struct Frame {
    uint8_t type;
    std::vector<uint8_t> payload;
};

// Encode frame → bytes
std::vector<uint8_t> encode_frame(uint8_t msg_type, const std::vector<uint8_t>& payload);

// Decode frame from raw buffer (not socket yet)
bool decode_frame(
    const std::vector<uint8_t>& buffer,
    Frame& out_frame
);

}