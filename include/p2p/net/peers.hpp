#pragma once

#include <fstream>
#include <string>
#include <vector>

enum class AddressType { IPv4 = 0, IPv6 = 1 };

struct Peer {
    AddressType type;
    std::string address;
    std::string port;
};

class PeerList {
  private:
    std::filebuf m_clientFD;
    std::vector<Peer> m_peers;

  public:
    int add_peer(Peer) noexcept;
    int retrieve_central_peers();
    std::vector<Peer> get_peers();
};
