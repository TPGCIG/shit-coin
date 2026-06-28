#pragma once

#include "peers.hpp"

#include <cstdint>

enum class PacketType {
    RequestPeerList,

    PeerList,
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
    char address[32];
    char port[4];
};

struct PeerListPacket {
    uint32_t peer_count;
    PeerPacket peers[1];
};

#pragma pack(pop)

PacketHeader build_header_pkt(PacketType, uint32_t);
PeerPacket build_peer_data_pkt(Peer);
PeerListPacket build_peer_list_pkt(std::vector<PeerPacket>);

PeerList unpack_peer_list_pkt(const PeerListPacket *);
PeerListPacket deserialise_peer_list_pkt(const std::byte *);
PacketHeader deserialise_header_pkt(void *);
