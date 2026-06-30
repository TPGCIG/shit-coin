#pragma once

#include "NetworkTypes.hpp"
#include "packets.hpp"

#include <fstream>
#include <iostream>

class PeerList {
  private:
    std::filebuf m_clientFD;
    std::vector<Peer> m_peers;

  public:
    int add_peer(Peer) noexcept;
    int retrieve_central_peers();
    const std::vector<Peer> &get_peers() const;
};

class Node {
  private:
    PeerList peers{};
    ProtocolEngine proto_engine;

    void send_peers(Peer);

  public:
    // this might be a bit excessive but im no OOP expert so ive converged
    // on a pattern where listener does its own thing and just sends information
    // up to the packet_ctrl. Then, the ctrl is able to submit peers to the peer
    // list without knowing what the Peer concept is. I think this is fine?
    // packet ctrl can maybe know what peers are since they are so conceptually
    // intertwined but whatever i guess. this works for now.
    //
    // FMI (for my information) this is kinda gross since peer is passed up as
    // its components rather than a peer struct but i think this is idiomatic?
    //
    // can refactor through hand-defining functions but cant be fucked.
    //
    // look at .cpp definitions
    Node(std::string, std::string);

    // ============= this is for debugging ====================

    void d_add_peer(AddressType t, std::string a, std::string p) {
        Peer peer{t, a, p};
        this->peers.add_peer(peer);
    };
};
