#pragma once
#include "../network/connection.h"
#include "../../services/master/master_logic.h"
#include "../messages/client_master.h"

class Dispatcher {
public:
    explicit Dispatcher(MasterLogic& logic) : master(logic) {}

    void handle(Connection& conn, const protocol::Frame& frame) {
        if (frame.type == 1) { // Type 1: CreateFile
            auto req = msg::CreateFileRequest::deserialize(frame.payload);
            bool success = master.createFile(req.filename);
            
            // Send Response back
            std::vector<uint8_t> resp = { static_cast<uint8_t>(success) };
            conn.sendFrame(2, resp); // Type 2: MasterToClient Response
        }
    }

private:
    MasterLogic& master;
};