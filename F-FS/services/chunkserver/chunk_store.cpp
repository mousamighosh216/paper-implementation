#include "chunk_store.h"
#include <iostream>

ChunkStore::ChunkStore(const std::string& data_dir) : root_path(data_dir) {
    std::filesystem::create_directories(root_path);
}

std::filesystem::path ChunkStore::getChunkPath(uint64_t handle) {
    return root_path / (std::to_string(handle) + ".chunk");
}

bool ChunkStore::write(uint64_t handle, uint32_t offset, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(store_mutex);
    
    // Open in binary mode, "in | out" allows seeking and partial writing
    std::fstream fs(getChunkPath(handle), std::ios::binary | std::ios::in | std::ios::out);
    
    // If file doesn't exist, create it
    if (!fs.is_open()) {
        fs.open(getChunkPath(handle), std::ios::binary | std::ios::out);
    }

    fs.seekp(offset);
    fs.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    if (fs.fail()) return false;

    // Update the checksums for the blocks we just touched
    updateChecksum(handle, offset, data);
    return true;
}

std::vector<uint8_t> ChunkStore::read(uint64_t handle, uint32_t offset, uint32_t length) {
    std::lock_guard<std::mutex> lock(store_mutex);
    
    std::ifstream ifs(getChunkPath(handle), std::ios::binary);
    if (!ifs) return {};

    std::vector<uint8_t> buffer(length);
    ifs.seekg(offset);
    ifs.read(reinterpret_cast<char*>(buffer.data()), length);

    // GFS Rule: Always verify checksum on read
    if (!verifyChecksum(handle, offset, buffer)) {
        std::cerr << "CRITICAL: Checksum mismatch on chunk " << handle << std::endl;
        return {}; // Or throw corruption error
    }

    return buffer;
}

void ChunkStore::updateChecksum(
    uint64_t chunk_id,
    uint32_t block_index,
    const std::vector<uint8_t>& data
) {

    // TODO
}

bool ChunkStore::verifyChecksum(
    uint64_t chunk_id,
    uint32_t block_index,
    const std::vector<uint8_t>& data
) {

    return true;
}