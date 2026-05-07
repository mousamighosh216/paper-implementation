#include "log_manager.h"
#include <iostream>

LogManager::LogManager(const std::string& log_path) : path(log_path) {
    // Open in binary append mode
    log_file.open(path, std::ios::binary | std::ios::app);
}

LogManager::~LogManager() {
    if (log_file.is_open()) log_file.close();
}

bool LogManager::append(OpType type, const std::vector<uint8_t>& payload) {
    std::lock_guard<std::mutex> lock(log_mutex);
    
    Serializer s;
    s.write_u64(payload.size()); // Length prefix for the entry
    
    try {
        // 1. Write metadata: [Type (1B)][PayloadLen (8B)]
        uint8_t type_byte = static_cast<uint8_t>(type);
        log_file.write(reinterpret_cast<char*>(&type_byte), 1);
        
        // 2. Write Payload Length
        uint64_t len = payload.size();
        log_file.write(reinterpret_cast<char*>(&len), 8); // Manual LE write preferred in production
        
        // 3. Write Payload
        log_file.write(reinterpret_cast<const char*>(payload.data()), payload.size());
        
        // 4. Force OS to push to physical disk (The "Durability" part)
        log_file.flush(); 
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<OpLogEntry> LogManager::replay() {
    std::vector<OpLogEntry> entries;
    std::ifstream reader(path, std::ios::binary);
    
    if (!reader.is_open()) return entries;

    while (reader.peek() != EOF) {
        uint8_t type_byte;
        uint64_t len;
        
        reader.read(reinterpret_cast<char*>(&type_byte), 1);
        reader.read(reinterpret_cast<char*>(&len), 8);
        
        std::vector<uint8_t> payload(len);
        reader.read(reinterpret_cast<char*>(payload.data()), len);
        
        entries.push_back({static_cast<OpType>(type_byte), payload});
    }
    return entries;
}