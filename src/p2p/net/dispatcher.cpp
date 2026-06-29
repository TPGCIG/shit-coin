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
    auto hdr = build_header_pkt(PacketType::RequestPeerList, 0);
    auto hdr_buf = serialise_header_pkt(hdr);

    // send it
    return this->send_to_peer<PacketHeader>(peer, hdr_buf);
}

int Dispatcher::send_peers(Peer peer, PeerList *peer_list) {
    auto plp = build_peer_list_pkt(peer_list);
    auto plp_buf = serialise_peer_pkts(plp);

    auto hdr = build_header_pkt(PacketType::PeerPacket, plp_buf.size());
    auto hdr_buf = serialise_header_pkt(hdr);

    return this->send_to_peer<PeerListPacket>(peer, hdr_buf, plp_buf);
}
