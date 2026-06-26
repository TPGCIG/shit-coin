#include "p2p/peers.hpp"
#include "csv.h"

Peer::Peer(std::string address, std::string port, std::string co_lat, std::string co_lon)
        : address(address)
        , port(port)
        , co_lat(co_lat)
        , co_lon(co_lon)
{};


int PeerList::add_peer(std::string address, std::string port, std::string co_lat, std::string co_lon) noexcept {
    peers.emplace_back(address, port, co_lat, co_lon);
    return peers.size();
};


void PeerList::retrieve_central_peers() {
    io::CSVReader<4> master_peers("peers/master_clients.csv");
    master_peers.read_header(io::ignore_extra_column, "address", "port", "coord_lat", "coord_lon");
    std::string address, port, co_lat, co_lon;

    while (master_peers.read_row(address, port, co_lat, co_lon)) {
        peers.emplace_back(address, port, co_lat, co_lon);
    }
};


