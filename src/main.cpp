#include "p2p/net/node.hpp"

#define LH_A "localhost"
#define LH_P "9876"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char *argv[]) {
    (void)argc;

    Peer pr{AddressType::IPv4, LH_A, LH_P};
    Peer pr2{AddressType::IPv6, "hongkong", "1111"};
    PeerList pl;

    Node node("localhost", argv[1]);
    if (!(strcmp(argv[2], "d"))) {
        node.d_add_peer(AddressType::IPv6, "hongkong", "1111");
        node.d_add_peer(AddressType::IPv6, "china", "6969");
        node.d_add_peer(AddressType::IPv6, "ninja", "4200");
    }
    getchar();
    return 0;
}
