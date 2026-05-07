#include "../../services/chunkserver/chunk_store.h"
#include <iostream>
#include <cassert>
#include <filesystem>

void test_chunk_integrity() {
    std::string test_dir = "test_chunks_data";
    if (std::filesystem::exists(test_dir)) std::filesystem::remove_all(test_dir);

    ChunkStore store(test_dir);
    uint64_t handle = 1001;
    std::vector<uint8_t> original_data = {'G', 'O', 'O', 'G', 'L', 'E'};

    // 1. Write Data
    std::cout << "Writing data to chunk 1001..." << std::endl;
    assert(store.write(handle, 0, original_data));

    // 2. Normal Read (Should pass)
    std::cout << "Reading data back (Normal)..." << std::endl;
    auto read_data = store.read(handle, 0, original_data.size());
    assert(read_data == original_data);
    std::cout << "✅ Normal Read Passed." << std::endl;

    // 3. Simulate Bit Rot (Manual Corruption)
    // We open the file directly without using the ChunkStore class
    std::cout << "Simulating Bit Rot (Corrupting file on disk)..." << std::endl;
    std::string filepath = test_dir + "/1001.chunk";
    std::fstream fs(filepath, std::ios::binary | std::ios::in | std::ios::out);
    fs.seekp(0);
    fs.put('X'); // Change 'G' to 'X'
    fs.close();

    // 4. Read Corrupted Data (Should fail checksum)
    std::cout << "Reading corrupted data..." << std::endl;
    auto corrupted_data = store.read(handle, 0, original_data.size());
    
    // The read should return an empty vector because verifyChecksum failed
    assert(corrupted_data.empty());
    std::cout << "✅ Bit Rot Detected Successfully!" << std::endl;

    std::filesystem::remove_all(test_dir);
}

int main() {
    try {
        test_chunk_integrity();
        std::cout << "\n🚀 CHUNK STORE TEST PASSED!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}