#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include "../../storage/oplog/log_manager.h"

using ChunkHandle = uint64_t;

struct ChunkMetadata {
    uint64_t version;
    std::vector<std::string> locations; // IP:Port of ChunkServers
    // Future: primary_lease, expiration
};

class MasterLogic {
public:
    explicit MasterLogic(LogManager& oplog);

    // --- File Operations ---
    bool createFile(const std::string& path);
    std::optional<std::vector<ChunkHandle>> getFileChunks(const std::string& path);
    
    // --- Chunk Operations ---
    // Picks 3 servers and assigns a new handle
    ChunkHandle allocateChunk(const std::string& path, const std::vector<std::string>& available_servers);

    // --- Recovery ---
    void recover();

private:
    LogManager& oplog_ref;
    
    // The "Source of Truth" in RAM
    std::unordered_map<std::string, std::vector<ChunkHandle>> file_table;
    std::unordered_map<ChunkHandle, ChunkMetadata> chunk_table;
    
    ChunkHandle next_handle = 1000; // Start handle range
    mutable std::shared_mutex table_mutex; 

    // Internal helper for state transitions without logging
    void applyOperation(OpType type, const std::vector<uint8_t>& payload);
};