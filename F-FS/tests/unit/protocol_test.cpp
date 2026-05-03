#include <cassert>
#include <vector>
#include <iostream>

#include "../../core/protocol/frame.h"

int main() {
    // Step 1: Create test payload
    uint8_t type = 5;
    std::vector<uint8_t> payload = {1, 2, 3};

    // Step 2: Encode frame
    std::vector<uint8_t> encoded = protocol::encode_frame(type, payload);

    // Step 3: Decode frame
    protocol::Frame decoded;
    bool ok = protocol::decode_frame(encoded, decoded);

    // Step 4: Assertions
    assert(ok && "Decoding failed");
    assert(decoded.type == type && "Message type mismatch");
    assert(decoded.payload == payload && "Payload mismatch");

    std::cout << "✅ protocol_test passed\n";

    return 0;
}