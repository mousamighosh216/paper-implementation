#pragma once
#include "../serialization/codec.h"

namespace msg {

// CS -> CS: "Here is the data, forward it to the next guys in the list"
struct ForwardData {
    uint64_t handle;
    std::vector<uint8_t> data;
    std::vector<std::string> remaining_chain;

    std::vector<uint8_t> serialize() const {
        Serializer s;
        s.write_u64(handle);
        s.write_vector(data); // Vector of raw bytes
        s.write_vector(remaining_chain); // Vector of IP strings
        return s.buffer;
    }
};

} // namespace msg