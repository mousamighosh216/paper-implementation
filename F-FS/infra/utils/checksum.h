#pragma once
#include <vector>
#include <cstdint>

namespace utils {
    // A simple hash for testing; in production, use a library like zlib for real CRC32
    inline uint32_t calculate_checksum(const std::vector<uint8_t>& data) {
        uint32_t crc = 0xFFFFFFFF;
        for (auto byte : data) {
            crc = (crc ^ byte) * 0x04C11DB7; 
        }
        return crc;
    }
}