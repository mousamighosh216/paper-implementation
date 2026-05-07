#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>

struct BufferedData {
    std::vector<uint8_t> data;
    std::chrono::steady_clock::time_point timestamp;
};

class DataBuffer {
public:
    // Store data sent by client, identified by a unique checksum or ID
    void push(const std::string& data_id, const std::vector<uint8_t>& data) {
        std::lock_guard<std::mutex> lock(mtx);
        buffer[data_id] = { data, std::chrono::steady_clock::now() };
    }

    std::vector<uint8_t> pop(const std::string& data_id) {
        std::lock_guard<std::mutex> lock(mtx);
        if (buffer.find(data_id) == buffer.end()) return {};
        auto data = buffer[data_id].data;
        buffer.erase(data_id); // Clean up after use
        return data;
    }

private:
    std::unordered_map<std::string, BufferedData> buffer;
    std::mutex mtx;
};