#pragma once

#include "p2p/net/scopedsocket.hpp"

#include <vector>
#include <thread>
#include <netdb.h>

class Listener {
private:
    std::vector<std::jthread> m_workers;
    int handle_client(ScopedSocket);
public:
    void start_listening();
};
