#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include "../../infra/utils/checksum.h"

class ChunkStore {
public:
    explicit ChunkStore(const std::string& data_dir);

    // Creates or appends data to a chunk
    bool write(uint64_t handle, uint32_t offset, const std::vector<uint8_t>& data);

    // Reads data and verifies checksum
    std::vector<uint8_t> read(uint64_t handle, uint32_t offset, uint32_t length);

    // Returns a list of all ChunkHandles found on disk (for Master registration)
    std::vector<uint64_t> scanChunks();

private:
    std::filesystem::path root_path;
    std::mutex store_mutex;

    // Internal helpers
    std::filesystem::path getChunkPath(uint64_t handle);
    std::filesystem::path getChecksumPath(uint64_t handle);
    
    void updateChecksum(uint64_t handle, uint32_t offset, const std::vector<uint8_t>& data);
    bool verifyChecksum(uint64_t handle, uint32_t offset, const std::vector<uint8_t>& data);
};