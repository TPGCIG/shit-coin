
#pragma once

#include "p2p/net/packets.hpp"
#include "p2p/net/scopedsocket.hpp"
#include "p2p/peers.hpp"

#include <thread>
#include <vector>

void *get_in_addr(sockaddr *);

// now we play the client

class Dispatcher {
  private:
    std::vector<std::jthread> m_workers;
    // int send_to_peer(Peer, Packet);
    //  TODO: create template or something to handle various packet types.

  public:
    int request_peers(Peer);
    int provide_peers(Peer, PeerList);
};
