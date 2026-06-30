#include "p2p/net/node.hpp"
#include "p2p/net/NetworkTypes.hpp"

#include "csv.h"

#include <chrono>
#include <iostream>

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

const std::vector<Peer> &PeerList::get_peers() const { return m_peers; }

int PeerList::add_peer(Peer peer) noexcept {
    auto it = std::find(this->m_peers.begin(), this->m_peers.end(), peer);

    if (it == this->get_peers().end()) {
        m_peers.push_back(peer);
    }
    return m_peers.size();
};

void Node::send_peers(Peer peer) {
    proto_engine.send_peers(peer, peers.get_peers());
}

Node::Node(std::string addr, std::string port)
    : proto_engine(
          addr, port, [this](Peer peer) { this->peers.add_peer(peer); },
          [this](Peer peer) { this->send_peers(peer); }) {

    int peer_count{this->peers.retrieve_central_peers()};
    std::cout << "received " << peer_count << " peers locally\n";
    if (!strcmp(port.data(), "9876")) {
        this->d_add_peer(AddressType::IPv6, std::string("hongkong"),
                         std::string("1111"));
        this->d_add_peer(AddressType::IPv6, std::string("china"),
                         std::string("2222"));
        this->d_add_peer(AddressType::IPv6, std::string("boomboom"),
                         std::string("3333"));
    }

    std::cout << "requesting peers from: \n";
    for (auto peer : peers.get_peers()) {
        // not us
        if (strcmp(peer.port.data(), port.data()) == 0)
            continue;

        std::cout << "    | address: " << peer.address << "\n"
                  << "    | port: " << peer.port << "\n\n";
        proto_engine.request_peers(peer);
    }

    for (int i{}; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << '.';
        std::flush(std::cout);
    }
    std::cout << "\n";

    std::cout << "received " << this->peers.get_peers().size() << " peers from "
              << peer_count << " peers\n";

    for (Peer peer : peers.get_peers()) {
        std::cout << peer.address << " " << peer.port << "\n";
    }

    std::cout << "done printing peers\n";
};
