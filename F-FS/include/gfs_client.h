#pragma once
#include "../core/network/connection.h"
#include "../core/messages/client_master.h"

class GFSClient {
public:
    GFSClient(std::string master_ip, uint16_t port);

    bool write(const std::string& filename, const std::vector<uint8_t>& data) {
        // 1. Ask Master for Chunk Locations
        auto locs = getLocations(filename);
        
        // 2. Push data to all replicas (Data Flow)
        std::string data_id = "uuid_1234"; // Generated ID
        for (auto& ip : locs.locations) {
            pushData(ip, data_id, data);
        }

        // 3. Send Commit to Primary (Control Flow)
        return commitWrite(locs.locations[0], locs.handle, data_id);
    }

private:
    // Internal network helpers...
};