#include "p2p/net/node.hpp"

Node::Node() {
    peer_list.retrieve_central_peers();

    // dispatcher.request_peers();
}
