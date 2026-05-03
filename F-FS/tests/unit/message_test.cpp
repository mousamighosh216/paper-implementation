#include "../../core/messages/client_master.h"
#include <cassert>
#include <iostream>
#include <vector>

void test_create_file_request() {
    msg::CreateFileRequest original;
    original.filename = "/data/logs/syslog.txt";

    // Serialize
    std::vector<uint8_t> buffer = original.serialize();

    // Deserialize (Manually using the logic we'll put in the dispatcher)
    Deserializer d(buffer);
    std::string decoded_name = d.read_string();

    assert(decoded_name == original.filename);
    std::cout << "✅ CreateFileRequest Test Passed!" << std::endl;
}

void test_chunk_locations_response() {
    msg::ChunkLocationsResponse original;
    original.handle = 0xDEADBEEFCAF1BABE; // Large 64-bit ID
    original.locations = {"192.168.1.10:6001", "192.168.1.11:6001", "192.168.1.12:6001"};

    // Serialize
    std::vector<uint8_t> buffer = original.serialize();

    // Deserialize
    msg::ChunkLocationsResponse decoded = msg::ChunkLocationsResponse::deserialize(buffer);

    // Verify
    assert(decoded.handle == original.handle);
    assert(decoded.locations.size() == 3);
    assert(decoded.locations[1] == "192.168.1.11:6001");
    
    std::cout << "✅ ChunkLocationsResponse Test Passed!" << std::endl;
}

int main() {
    try {
        test_create_file_request();
        test_chunk_locations_response();
        std::cout << "\n🚀 ALL MESSAGE TESTS PASSED!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}