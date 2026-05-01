#include "../core/protocol/frame.h"
#include <iostream>
#include <cassert>

using namespace protocol;

int main() {
    std::vector<uint8_t> payload = {1, 2, 3, 4, 5};

    auto encoded = encode_frame(5, payload);

    Frame decoded;
    bool ok = decode_frame(encoded, decoded);

    assert(ok);
    assert(decoded.type == 5);
    assert(decoded.payload == payload);

    std::cout << "Frame test passed ✅\n";
}