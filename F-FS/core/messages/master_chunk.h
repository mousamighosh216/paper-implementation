#pragma once
#include "../serialization/codec.h"

namespace msg {

// Master -> ChunkServer: "Bump this chunk's version"
struct UpdateVersion {
    uint64_t handle;
    uint64_t new_version;

    std::vector<uint8_t> serialize() const {
        Serializer s;
        s.write_u64(handle);
        s.write_u64(new_version);
        return s.buffer;
    }
};

// Master -> ChunkServer: "Clone chunk A to handle B" (COW Snapshot)
struct CopyChunk {
    uint64_t src_handle;
    uint64_t dst_handle;

    std::vector<uint8_t> serialize() const {
        Serializer s;
        s.write_u64(src_handle);
        s.write_u64(dst_handle);
        return s.buffer;
    }
};

} // namespace msg