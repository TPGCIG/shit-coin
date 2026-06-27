#include "p2p/net/dispatcher.hpp"
#include "p2p/net/node.hpp"
#include "p2p/peers.hpp"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    (void)argc;

    Listener listener;
    Dispatcher dispatcher;
    PeerList pl;

    (void)pl.retrieve_central_peers();
    std::vector peers = pl.get_peers();

    if (std::string(argv[1]) == "l") {
        listener.start_listening();
    } else {
        dispatcher.connect_to_peer(peers[0]);
    }

    std::cout << "end main" << std::endl;

    return 0;
}
