#pragma once

#include "NetworkTypes.hpp"
#include "dispatcher.hpp"
#include "listener.hpp"

#include <cstdint>
#include <functional>

enum class PacketType {
    Nothing,
    RequestPeerList,
    PeerPacket,
};

#pragma pack(push, 1)

// Signal is an initial yell down the wire to indicate what comes next.
struct PeerPacket {
    uint8_t type;
    char address[33];
    char port[5];
};

struct PacketHeader {
    uint8_t packet_type;
    uint32_t length;
    PeerPacket sender;
};

#pragma pack(pop)

std::vector<uint8_t> serialise_peer_pkts(std::vector<PeerPacket>);

PeerPacket build_peer_data_pkt(Peer);
std::vector<PeerPacket> build_peer_list_pkt(std::vector<Peer>);

PacketHeader deserialise_header_pkt(const std::byte *);

class ProtocolEngine {
  private:
    using SubmitPeer = std::function<void(Peer)>;
    using SignalRequestPeers = std::function<void(Peer)>;

    PeerPacket me;

    Listener listener;
    Dispatcher dispatcher;

    SubmitPeer submit_peer;
    SignalRequestPeers signal_request_peers;

    std::jthread listener_thread;

    PacketType m_pkt_type{PacketType::Nothing};

    /*
     * ======= deserialising ===========
     */

    void parse_peer_pkts(const std::byte *, size_t);
    size_t parse_header(const std::byte *);
    void parse_body(const std::byte *, size_t len);

    /*
     * serialising is done via static functions.
     */
    PacketHeader build_header_pkt(PacketType, uint32_t);
    std::vector<uint8_t> serialise_header_pkt(PacketHeader);

    void handle_header(PacketHeader);

  public:
    ProtocolEngine(std::string, std::string, SubmitPeer, SignalRequestPeers);

    int send_peers(Peer, std::vector<Peer>);
    void request_peers(Peer);
};
