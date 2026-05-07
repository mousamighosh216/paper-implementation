#include "master_logic.h"

MasterLogic::MasterLogic(LogManager& oplog) : oplog_ref(oplog) {}

bool MasterLogic::createFile(const std::string& path) {
    std::unique_lock lock(table_mutex);
    
    if (file_table.find(path) != file_table.end()) return false;

    // 1. Durability First: Serialize and Log
    Serializer s;
    s.write_string(path);
    if (!oplog_ref.append(OpType::CREATE_FILE, s.buffer)) return false;

    // 2. Mutate Memory
    file_table[path] = {};
    return true;
}

ChunkHandle MasterLogic::allocateChunk(const std::string& path, const std::vector<std::string>& servers) {
    std::unique_lock lock(table_mutex);
    
    if (file_table.find(path) == file_table.end()) return 0;

    ChunkHandle new_h = next_handle++;
    uint64_t initial_version = 1; // Start at 1
    
    // 1. Pick Replicas
    std::vector<std::string> chosen_replicas;
    for(int i=0; i<3 && i<servers.size(); ++i) chosen_replicas.push_back(servers[i]);

    // 2. Grant the initial lease to the first replica
    auto [primary, secondaries] = lease_manager.grantOrGetLease(new_h, chosen_replicas);

    // 3. Log the "Birth" of the chunk with its version and primary
    Serializer s;
    s.write_string(path);
    s.write_u64(new_h);
    s.write_u64(initial_version);
    s.write_vector(chosen_replicas);
    oplog_ref.append(OpType::ADD_CHUNK, s.buffer);

    // 4. Update Memory state
    file_table[path].push_back(new_h);
    chunk_table[new_h] = { initial_version, chosen_replicas };
    
    return new_h;
}

void MasterLogic::recover() {
    auto entries = oplog_ref.replay();
    for (const auto& entry : entries) {
        applyOperation(entry.type, entry.payload);
    }
}

void MasterLogic::applyOperation(OpType type, const std::vector<uint8_t>& payload) {
    Deserializer d(payload);
    if (type == OpType::CREATE_FILE) {
        file_table[d.read_string()] = {};
    } else if (type == OpType::ADD_CHUNK) {
        std::string path = d.read_string();
        ChunkHandle h = d.read_u64();
        auto locs = d.read_vector<std::string>();
        file_table[path].push_back(h);
        chunk_table[h] = {0, locs};
        if (h >= next_handle) next_handle = h + 1;
    }
}