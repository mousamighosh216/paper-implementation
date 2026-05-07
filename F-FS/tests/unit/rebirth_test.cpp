#include "../../services/master/master_logic.h"
#include <iostream>
#include <cassert>
#include <filesystem>

void run_rebirth_test() {
    std::string log_path = "rebirth_oplog.bin";
    if (std::filesystem::exists(log_path)) std::filesystem::remove(log_path);

    std::vector<std::string> mock_servers = {"10.0.0.1:6000", "10.0.0.2:6000", "10.0.0.3:6000"};
    std::vector<uint64_t> saved_handles;

    {
        // --- STAGE 1: THE LIFE ---
        LogManager oplog(log_path);
        MasterLogic master(oplog);

        std::cout << "Creating 5 files with 2 chunks each..." << std::endl;
        for (int i = 0; i < 5; ++i) {
            std::string filename = "/file_" + std::to_string(i) + ".dat";
            master.createFile(filename);
            
            // Allocate 2 chunks and save handles for verification
            saved_handles.push_back(master.allocateChunk(filename, mock_servers));
            saved_handles.push_back(master.allocateChunk(filename, mock_servers));
        }
        std::cout << "State saved to OpLog. Master object being destroyed (Crashing)..." << std::endl;
    } 

    // At this point, the 'master' and 'oplog' objects are out of scope (destroyed)
    
    {
        // --- STAGE 2: THE REBIRTH ---
        LogManager new_oplog(log_path);
        MasterLogic new_master(new_oplog);

        std::cout << "Restarting Master and recovering..." << std::endl;
        new_master.recover();

        std::cout << "Verifying metadata integrity..." << std::endl;
        int handle_idx = 0;
        for (int i = 0; i < 5; ++i) {
            std::string filename = "/file_" + std::to_string(i) + ".dat";
            auto chunks_opt = new_master.getFileChunks(filename);
            
            assert(chunks_opt.has_value());
            assert(chunks_opt->size() == 2);
            
            // Verify IDs match exactly in order
            assert((*chunks_opt)[0] == saved_handles[handle_idx++]);
            assert((*chunks_opt)[1] == saved_handles[handle_idx++]);
            
            std::cout << "✅ " << filename << " recovered correctly." << std::endl;
        }
    }

    std::cout << "\n🚀 REBIRTH TEST PASSED! The Master is deterministic." << std::endl;
    std::filesystem::remove(log_path);
}

int main() {
    run_rebirth_test();
    return 0;
}