#pragma once
#include "../serialization/codec.h"

namespace msg {

// Client -> Master: CreateFile
struct CreateFileRequest {
    std::string filename;

    std::vector<uint8_t> serialize() const {
        Serializer s;
        s.write_string(filename);
        return s.buffer;
    }

    static CreateFileRequest deserialize(const std::vector<uint8_t>& buf) {
        Deserializer d(buf);
        return { d.read_string() };
    }
};

// Master -> Client: ChunkLocations
struct ChunkLocationsResponse {
    uint64_t handle;
    std::vector<std::string> locations;

    // Fixed: Added the missing serialize member
    std::vector<uint8_t> serialize() const {
        Serializer s;
        s.write_u64(handle);
        s.write_vector(locations); 
        return s.buffer;
    }

    static ChunkLocationsResponse deserialize(const std::vector<uint8_t>& buf) {
        Deserializer d(buf);
        uint64_t h = d.read_u64();
        auto locs = d.read_vector<std::string>(); // Handled by template!
        return {h, locs};
    }
};
} // namespace msg