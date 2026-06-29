#pragma once

#include "dispatcher.hpp"
#include "listener.hpp"
#include "peers.hpp"

class Node {
  public:
    // this might be a bit excessive but im no OOP expert so ive converged
    // on a pattern where listener does its own thing and just sends information
    // up to the packet_ctrl. Then, the ctrl is able to submit peers to the peer
    // list without knowing what the Peer concept is. I think this is fine?
    // packet ctrl can maybe know what peers are since they are so conceptually
    // intertwined but whatever i guess. this works for now.
    //
    // FMI (for my information) this is kinda gross since peer is passed up as
    // its components rather than a peer struct but i think this is idiomatic?
    //
    // can refactor through hand-defining functions but cant be fucked.
    Node()
        : listener(
              // parse_header lambda - for parsing header packets in the ctrl.
              [this](const std::byte *buf) {
                  return this->packet_ctrl.parse_header(buf);
              },
              // parse_body lambda - for parsing body packets given a specific
              // state. we save the state that the header tells us, not too
              // race-y since this is TCP and bound to one listener.
              [this](const std::byte *buf, size_t len) {
                  this->packet_ctrl.parse_body(buf, len);
              }),
          // for packet_ctrl to submit peers - this one is pretty intuitive i
          // guess.
          packet_ctrl([this](int type, std::string addr, std::string port) {
              Peer peer{static_cast<AddressType>(type), addr, port};
              peers.add_peer(peer);
          }) {};

  private:
    PeerList peers{};
    Listener listener;
    Dispatcher dispatcher{};
    PacketController packet_ctrl;
};
