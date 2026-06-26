
#pragma once

#include "p2p/net/scopedsocket.hpp"
#include "p2p/peers.hpp"
#include <cstdint>
#include <vector>
#include <thread>


void *get_in_addr(sockaddr *);

// will change just dumb packet for testing
struct __attribute__((packed)) Packet {
    uint8_t nature;
    uint64_t payload;
};

// now we play the client

class Dispatcher {
private:
    std::vector<std::jthread> m_workers;
public:
    int connect_to_peer(Peer);




};



