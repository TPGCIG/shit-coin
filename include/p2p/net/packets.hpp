#pragma once

#include "peers.hpp"

#include <cstdint>

enum class PacketType {
    RequestPeerList,

    PeerPacket,
};

constexpr bool is_bodyless(PacketType type) {
    switch (type) {
    case PacketType::RequestPeerList:
        return true;
    default:
        return false;
    }
}

#pragma pack(push, 1)

// Signal is an initial yell down the wire to indicate what comes next.
struct PacketHeader {
    uint8_t packet_type;
    uint32_t length;
};

struct PeerPacket {
    uint8_t type;
    char address[33];
    char port[5];
};

struct PeerListPacket {
    uint32_t peer_count;
    PeerPacket peers[1];
};

#pragma pack(pop)

std::vector<uint8_t> serialise_header_pkt(PacketHeader);
std::vector<uint8_t> serialise_peer_pkts(std::vector<PeerPacket>);

PacketHeader build_header_pkt(PacketType, uint32_t);
PeerPacket build_peer_data_pkt(Peer);
std::vector<PeerPacket> build_peer_list_pkt(PeerList *);

PeerList deserialise_peer_pkts(const std::byte *, size_t);
PacketHeader deserialise_header_pkt(const std::byte *);
