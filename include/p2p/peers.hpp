#pragma once

#include <fstream>
#include <string>
#include <vector>



struct Peer {
    std::string address;
    std::string port;
    // tracking coordinates for spacial connections
    std::string co_lat;
    std::string co_lon;
};

class PeerList {
private:
    std::filebuf m_clientFD;
    std::vector<Peer> m_peers;

public:
    int add_peer(std::string, std::string, std::string, std::string) noexcept;
    int retrieve_central_peers();
};
