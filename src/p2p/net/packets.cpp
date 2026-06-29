#include "p2p/net/packets.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>

std::vector<uint8_t> serialise_header_pkt(PacketHeader header) {
    std::vector<uint8_t> buffer;

    const PacketHeader serialised_pkt{header.packet_type, htonl(header.length)};

    const uint8_t *byte_ptr =
        reinterpret_cast<const uint8_t *>(&serialised_pkt);
    buffer.insert(buffer.end(), byte_ptr, byte_ptr + sizeof(PacketHeader));
    return buffer;
}

std::vector<uint8_t> serialise_peer_pkts(std::vector<PeerPacket> peer_list) {
    std::vector<uint8_t> buffer;

    for (PeerPacket peer : peer_list) {
        const uint8_t *byte_ptr = reinterpret_cast<const uint8_t *>(&peer);
        buffer.insert(buffer.end(), byte_ptr, byte_ptr + sizeof(PeerPacket));
    }
    return buffer;
}

PacketHeader build_header_pkt(PacketType type, uint32_t length) {
    uint8_t pkt_type = static_cast<uint8_t>(type);

    PacketHeader header{.packet_type = pkt_type, .length = length};

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

std::vector<PeerPacket> build_peer_list_pkt(PeerList *peers) {
    std::vector<PeerPacket> peer_packets;

    for (Peer peer : peers->get_peers()) {
        peer_packets.push_back(build_peer_data_pkt(peer));
    }

    return peer_packets;
}

void PacketController::parse_peer_pkts(const std::byte *buf,
                                       size_t buf_length) {
    size_t offset{};
    size_t peer_count{buf_length / sizeof(PeerPacket)};

    for (size_t i{}; i < peer_count; ++i) {
        int type{};
        char addr[33];
        char port[5];

        std::memcpy(&type, buf + offset, sizeof(type));
        offset += sizeof(type);

        std::memcpy(&addr, buf + offset, sizeof(addr));
        offset += sizeof(addr);

        std::memcpy(&port, buf + offset, sizeof(port));
        offset += sizeof(port);

        this->submit_peer(type, static_cast<std::string>(addr),
                          static_cast<std::string>(port));
    }
}

PacketHeader deserialise_header_pkt(const std::byte *buf) {

    PacketHeader h;

    std::memcpy(&h, buf, sizeof h);

    h.length = ntohl(h.length);

    return h;
}

size_t PacketController::parse_header(const std::byte *buf) {
    PacketHeader hdr{deserialise_header_pkt(buf)};
    this->m_pkt_type = static_cast<PacketType>(
        hdr.packet_type); // incoming packet state is held
    return hdr.length;
}

void PacketController::parse_body(const std::byte *buf, size_t buf_len) {
    switch (this->m_pkt_type) {
    case PacketType::PeerPacket: {
        this->parse_peer_pkts(buf, buf_len);
        break;
    }
    default:
        std::cerr << "listener: catastrophic error - packet type\n";
        return;
    }
}
