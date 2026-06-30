#pragma once

#include <string>

enum class AddressType { IPv4 = 0, IPv6 = 1 };

struct Peer {
    AddressType type;
    std::string address;
    std::string port;
};
