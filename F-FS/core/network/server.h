#pragma once
#include <string>
#include <functional>
#include "connection.h"

class Server {
public:
    using HandlerFunc = std::function<void(Connection)>;

    Server(int port);
    
    // Starts the infinite accept loop
    void run(HandlerFunc handler);

private:
    int listen_sock;
    int port;
};