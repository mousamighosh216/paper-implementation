#include "../services/master/master_logic.h"
#include "../core/network/server.h"
#include "../core/rpc/dispatcher.h"

int main() {
    // 1. Initialize Durable Storage
    LogManager oplog("master_metadata.log");
    
    // 2. Initialize Brain & Recover from previous crash
    MasterLogic master(oplog);
    master.recover(); 
    
    // 3. Setup RPC Dispatcher
    Dispatcher dispatcher(master);

    // 4. Start Networking
    Server master_server(5000);
    master_server.run([&dispatcher](Connection conn) {
        while (true) {
            auto frame_opt = conn.receiveFrame();
            if (!frame_opt) break; // Client disconnected
            dispatcher.handle(conn, *frame_opt);
        }
    });

    return 0;
}