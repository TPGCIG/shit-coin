#include "p2p/net/peers.hpp"
#include "csv.h"

#include <iostream>

std::vector<Peer> PeerList::get_peers() { return m_peers; }

int PeerList::add_peer(Peer peer) noexcept {
    m_peers.push_back(peer);
    return m_peers.size();
};

int PeerList::retrieve_central_peers() {
    std::cout << "retrieving central peers...\n";

    io::CSVReader<2> master_peers("src/p2p/peers/master_clients.csv");
    master_peers.read_header(io::ignore_extra_column, "address", "port");

    std::string address, port;
    auto type = AddressType::IPv4;
    while (master_peers.read_row(address, port)) {
        m_peers.emplace_back(type, address, port);
    }

    return m_peers.size();
};
