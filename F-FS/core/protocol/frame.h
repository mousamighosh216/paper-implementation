// Responsibilities
// Define binary framing format
// Serialize/deserialize raw payload (NOT business objects)

#pragma once
#include <cstdint>
#include <vector>

namespace protocol {
    const uint8_t MAGIC[2] = {0x4A, 0x46}; // 'J' 'F'
    const uint8_t VERSION = 1;
    const size_t HEADER_SIZE = 8;

    struct Frame {
        uint8_t type;
        std::vector<uint8_t> payload;
    };

    // Serializes a type and payload into a full JF binary frame
    std::vector<uint8_t> encode_frame(uint8_t type, const std::vector<uint8_t>& payload);

    // Decodes bytes into a Frame object. Returns false if magic/version fails.
    bool decode_frame(const std::vector<uint8_t>& buffer, Frame& out_frame);
}