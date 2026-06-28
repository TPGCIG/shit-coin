#include "p2p/net/packets.hpp"

#include <arpa/inet.h>
#include <cstring>

PacketHeader build_header_pkt(PacketType type, uint32_t length) {
    uint8_t pkt_type = static_cast<uint8_t>(type);

    PacketHeader header{.packet_type = pkt_type, .length = htonl(length)};

    return header;
}

PeerPacket build_peer_data_pkt(Peer peer) {

    PeerPacket pd{};
    pd.type = static_cast<uint8_t>(peer.type);
    strncpy(pd.address, peer.address.data(), sizeof pd.address);

    pd.address[sizeof pd.address - 1] = '\0';
    strncpy(pd.port, peer.port.data(), sizeof pd.port);
    pd.port[sizeof pd.port - 1] = '\0';

    return pd;
}

PeerListPacket build_peer_list_pkt(PeerList peers) {
    size_t payload_size = peers.get_peers().size() * sizeof(PeerPacket);

    std::vector<PeerPacket> peer_packets;

    for (auto peer : peers.get_peers()) {
        peer_packets.push_back(build_peer_data_pkt(peer));
    }

    PeerListPacket plp{};

    plp.peer_count = htonl(peer_packets.size());
    std::memcpy(plp.peers, peer_packets.data(), payload_size);

    return plp;
}

PeerList unpack_peer_list_pkt(const PeerListPacket *pkt) {
    PeerList peers;

    for (size_t i = 0; i < ntohl(pkt->peer_count); ++i) {
        const PeerPacket &pp = pkt->peers[i];
        const auto type = static_cast<AddressType>(pp.type);
        const std::string address(pp.address);
        const std::string port(pp.port);

        peers.add_peer(type, address, port);
    }

    return peers;
}

PeerList deserialise_peer_list_pkt(const void *buf) {
    const auto *plp = static_cast<const PeerListPacket *>(buf);

    return unpack_peer_list_pkt(plp);
}

PacketHeader deserialise_header_pkt(void *buf) {

    PacketHeader h;

    std::memcpy(&h, buf, sizeof h);

    h.length = ntohl(h.length);

    return h;
}
