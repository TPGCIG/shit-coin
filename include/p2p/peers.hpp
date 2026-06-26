#pragma once

#include <fstream>
#include <string>
#include <vector>

class Peer {
private:
    std::string address;
    std::string port;
    // tracking coordinates for spacial connections
    std::string co_lat;
    std::string co_lon;

public:
    Peer(std::string, std::string, std::string, std::string);
};

class PeerList {
private:
    std::filebuf clientFD;
    std::vector<Peer> peers;

public:
    int add_peer(std::string, std::string, std::string, std::string) noexcept;
    void retrieve_central_peers();
};
