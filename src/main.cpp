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

    Dispatcher dp;
    Listener ls;

    Peer pr{AddressType::IPv4, LH_A, LH_P};

    if (strcmp(argv[1], "l") == 0) {
        ls.start_listening();
    } else {
        dp.request_peers(pr);
    }

    return 0;
}
