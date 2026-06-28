#include "p2p/net/dispatcher.hpp"
#include "p2p/net/packets.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int Dispatcher::request_peers(Peer peer) {
    /* Requests peers from a provided peer */

    // create the request Packet'
    auto header = build_header_pkt(PacketType::RequestPeerList, 0);

    // send it
    return this->send_to_peer<PacketHeader>(peer, header);
}

int Dispatcher::provide_peers(Peer peer, PeerList peer_list) {
    (void)peer;
    (void)peer_list;
    // Packet pkt{};

    // pkt.type = PacketType::PeerList;
    return 0;
}
