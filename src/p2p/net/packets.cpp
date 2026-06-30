#include "p2p/net/packets.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>

Peer parse_peer_pkt(PeerPacket peer_pkt) {
    Peer peer{static_cast<AddressType>(peer_pkt.type),
              std::string(peer_pkt.address), std::string(peer_pkt.port)};

    return peer;
}

std::vector<uint8_t> ProtocolEngine::serialise_header_pkt(PacketHeader header) {
    std::vector<uint8_t> buffer;

    const PacketHeader serialised_pkt{header.packet_type, htonl(header.length),
                                      this->me};

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

PacketHeader ProtocolEngine::build_header_pkt(PacketType type,
                                              uint32_t length) {
    uint8_t pkt_type = static_cast<uint8_t>(type);

    PacketHeader header{pkt_type, length, this->me};

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

std::vector<PeerPacket> build_peer_list_pkt(std::vector<Peer> peers) {
    std::vector<PeerPacket> peer_packets;

    for (Peer peer : peers) {
        peer_packets.push_back(build_peer_data_pkt(peer));
    }

    return peer_packets;
}

void ProtocolEngine::parse_peer_pkts(const std::byte *buf, size_t buf_length) {
    size_t offset{};
    size_t peer_count{buf_length / sizeof(PeerPacket)};

    for (size_t i{}; i < peer_count; ++i) {
        PeerPacket peer{};

        std::memcpy(&peer.type, buf + offset, sizeof(peer.type));
        offset += sizeof(peer.type);

        std::memcpy(peer.address, buf + offset, sizeof(peer.address));
        offset += sizeof(peer.address);

        std::memcpy(peer.port, buf + offset, sizeof(peer.port));
        offset += sizeof(peer.port);

        this->submit_peer(Peer{static_cast<AddressType>(peer.type),
                               std::string(peer.address),
                               std::string(peer.port)});
    }
}

PacketHeader deserialise_header_pkt(const std::byte *buf) {

    PacketHeader h;

    std::memcpy(&h, buf, sizeof h);

    h.length = ntohl(h.length);

    return h;
}

void ProtocolEngine::handle_header(PacketHeader header) {
    /* this is a weird one, supposed to be invoked and handle the state of
     * pkt_type. mainly made for multiplexing and handling request packets i
     * guess.
     */

    switch (static_cast<PacketType>(header.packet_type)) {
    case PacketType::RequestPeerList:
        signal_request_peers(parse_peer_pkt(header.sender));
    default:
        return;
    }
}

size_t ProtocolEngine::parse_header(const std::byte *buf) {
    PacketHeader hdr{deserialise_header_pkt(buf)};
    this->m_pkt_type = static_cast<PacketType>(
        hdr.packet_type); // incoming packet state is held

    this->handle_header(hdr);

    return hdr.length;
}

void ProtocolEngine::parse_body(const std::byte *buf, size_t buf_len) {
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

std::vector<uint8_t> combine_message(std::vector<uint8_t> hdr,
                                     std::vector<uint8_t> body) {
    hdr.reserve(hdr.size() + body.size());
    hdr.insert(hdr.end(), body.begin(), body.end());

    return hdr;
}

int ProtocolEngine::send_peers(Peer peer, std::vector<Peer> peer_list) {
    auto plp = build_peer_list_pkt(peer_list);
    auto plp_buf = serialise_peer_pkts(plp);

    auto hdr = build_header_pkt(PacketType::PeerPacket, plp_buf.size());
    auto hdr_buf = serialise_header_pkt(hdr);

    auto message = combine_message(hdr_buf, plp_buf);

    return this->dispatcher.send_to_endpoint(peer.address, peer.port, true,
                                             message);
}

void ProtocolEngine::request_peers(Peer peer) {
    /* Requests peers from a provided peer */

    // create the request Packet'
    auto hdr = build_header_pkt(PacketType::RequestPeerList, 0);
    auto hdr_buf = serialise_header_pkt(hdr);

    // send it
    this->dispatcher.send_to_endpoint(peer.address, peer.port, true, hdr_buf);
}

ProtocolEngine::ProtocolEngine(std::string addr, std::string port,
                               SubmitPeer submit_peer,
                               SignalRequestPeers signal_request_peers)
    : listener(
          addr, port,
          [this](const std::byte *buf) { return this->parse_header(buf); },

          [this](const std::byte *buf, size_t len) {
              this->parse_body(buf, len);
          }),
      submit_peer(submit_peer), signal_request_peers(signal_request_peers) {

    /* make ourselves */
    this->me = build_peer_data_pkt(Peer{AddressType::IPv6, addr, port});

    /* initialise the listener */
    std::promise<void> init_promise;
    std::future<void> init_future = init_promise.get_future();

    // using promises is a potential remove, i thought i had a bug where it
    // terminated before finishing but apparently not? but works so fuck it.
    this->listener_thread = std::jthread(
        [this, addr, port](std::promise<void> promise) mutable {
            this->listener.start_listening(addr, port, std::move(promise));
        },
        std::move(init_promise));

    init_future.wait();
    listener_thread.detach();
}
