#pragma once

#include "dispatcher.hpp"
#include "listener.hpp"

class Node {
    Listener listener;
    Dispatcher dispatcher;
    PeerList peer_list;

    Node();

    ~Node();
};
