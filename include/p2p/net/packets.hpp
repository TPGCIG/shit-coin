#pragma once

#include <cstdint>

enum class PacketSignal {
    RequestPeerList,
    PeerList,

};

#pragma pack(push, 1)

// will change just dumb packet for testing
struct Signal {
    uint8_t packet_type;
};

#pragma pack(pop)
