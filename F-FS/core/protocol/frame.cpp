#include "frame.h"
#include <cstring>

namespace protocol {

// helper: write uint32 in big endian
static void write_u32_be(uint8_t* buf, uint32_t val) {
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

// helper: read uint32 from big endian
static uint32_t read_u32_be(const uint8_t* buf) {
    return (uint32_t(buf[0]) << 24) |
           (uint32_t(buf[1]) << 16) |
           (uint32_t(buf[2]) << 8)  |
            uint32_t(buf[3]);
}

std::vector<uint8_t> encode_frame(uint8_t msg_type, const std::vector<uint8_t>& payload) {
    size_t total_size = HEADER_SIZE + payload.size();
    std::vector<uint8_t> buffer(total_size);

    // MAGIC
    buffer[0] = MAGIC[0];
    buffer[1] = MAGIC[1];

    // VERSION
    buffer[2] = VERSION;

    // TYPE
    buffer[3] = msg_type;

    // LENGTH (big endian)
    write_u32_be(&buffer[4], static_cast<uint32_t>(payload.size()));

    // PAYLOAD
    if (!payload.empty()) {
        std::memcpy(&buffer[HEADER_SIZE], payload.data(), payload.size());
    }

    return buffer;
}

bool decode_frame(const std::vector<uint8_t>& buffer, Frame& out_frame) {
    if (buffer.size() < HEADER_SIZE) return false;

    // MAGIC CHECK
    if (buffer[0] != MAGIC[0] || buffer[1] != MAGIC[1]) {
        return false;
    }

    // VERSION CHECK (optional strict)
    if (buffer[2] != VERSION) {
        return false;
    }

    uint8_t msg_type = buffer[3];
    uint32_t length = read_u32_be(&buffer[4]);

    if (length > MAX_FRAME_SIZE) return false;

    if (buffer.size() < HEADER_SIZE + length) return false;

    out_frame.type = msg_type;
    out_frame.payload.assign(
        buffer.begin() + HEADER_SIZE,
        buffer.begin() + HEADER_SIZE + length
    );

    return true;
}

}