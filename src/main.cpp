#include "p2p/net/dispatcher.hpp"
#include "p2p/net/listener.hpp"
#include "p2p/net/packets.hpp"
#include "p2p/net/peers.hpp"

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

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    Dispatcher dp;
    Listener ls;

    Peer pr{AddressType::IPv4, LH_A, LH_P};
    Peer pr2{AddressType::IPv6, "hongkong", "1111"};
    PeerList pl;
    pl.add_peer(AddressType::IPv4, LH_A, LH_P);
    pl.add_peer(AddressType::IPv6, "hongkkong", "1111");
    pl.add_peer(AddressType::IPv6, "china", "1211");
    if (strcmp(argv[1], "l") == 0) {
        ls.start_listening();
    } else {
        dp.request_peers(pr);
    }

    return 0;
}
