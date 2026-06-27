#include "p2p/peers.hpp"
#include "csv.h"

std::vector<Peer> PeerList::get_peers() {
  return m_peers;
}

int PeerList::add_peer(std::string address,
                       std::string port,
                       std::string co_lat,
                       std::string co_lon) noexcept {
  m_peers.emplace_back(address, port, co_lat, co_lon);
  return m_peers.size();
};

int PeerList::retrieve_central_peers() {
  io::CSVReader<4> master_peers("src/p2p/peers/master_clients.csv");
  master_peers.read_header(io::ignore_extra_column, "address", "port",
                           "coord_lat", "coord_lon");
  std::string address, port, co_lat, co_lon;

  while (master_peers.read_row(address, port, co_lat, co_lon)) {
    m_peers.emplace_back(address, port, co_lat, co_lon);
  }

  return m_peers.size();
};
