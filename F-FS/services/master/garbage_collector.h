#pragma once
#include "master_logic.h"
#include <thread>
#include <set>

class GarbageCollector {
public:
    explicit GarbageCollector(MasterLogic& logic) : master(logic), running(false) {}

    void start() {
        running = true;
        worker = std::thread(&GarbageCollector::scan_loop, this);
    }

private:
    void scan_loop() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::minutes(1));
            
            // 1. Collect all live handles from the file table
            auto live_handles = master.getAllLiveHandles();
            
            // 2. Compare with all known handles in the chunk table
            // 3. If a handle exists in chunk_table but NOT file_table, it's garbage.
            // 4. Mark for deletion in next heartbeat response.
        }
    }

    MasterLogic& master;
    std::thread worker;
    std::atomic<bool> running;
};