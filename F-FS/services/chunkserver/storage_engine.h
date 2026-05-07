#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

class StorageEngine {
public:
    explicit StorageEngine(std::string data_dir) : root(data_dir) {
        std::filesystem::create_directories(root / "chunks");
    }

    // Writes data to a physical .chunk file
    bool writeChunk(uint64_t handle, const std::vector<uint8_t>& data) {
        std::ofstream ofs(getPath(handle), std::ios::binary);
        if (!ofs) return false;
        ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    }

    // Reads data from a physical .chunk file
    std::vector<uint8_t> readChunk(uint64_t handle) {
        std::ifstream ifs(getPath(handle), std::ios::binary | std::ios::ate);
        if (!ifs) return {};
        
        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<uint8_t> buffer(size);
        ifs.read(reinterpret_cast<char*>(buffer.data()), size);
        return buffer;
    }

private:
    std::filesystem::path root;
    std::filesystem::path getPath(uint64_t handle) {
        return root / "chunks" / (std::to_string(handle) + ".chunk");
    }
};