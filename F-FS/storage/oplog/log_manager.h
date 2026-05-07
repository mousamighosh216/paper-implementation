#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include "../../core/serialization/codec.h"

// Define types of entries that can be logged
enum class OpType : uint8_t {
    CREATE_FILE = 1,
    DELETE_FILE = 2,
    ADD_CHUNK   = 3,
    SNAPSHOT    = 4
};

struct OpLogEntry {
    OpType type;
    std::vector<uint8_t> payload;
};

class LogManager {
public:
    explicit LogManager(const std::string& log_path);
    ~LogManager();

    // Appends a change to the disk. Blocks until flushed.
    bool append(OpType type, const std::vector<uint8_t>& payload);

    // Reads the entire log from disk to reconstruct state
    std::vector<OpLogEntry> replay();

    void rotate(const std::string& new_path);

private:
    std::string path;
    std::ofstream log_file;
    std::mutex log_mutex;
};