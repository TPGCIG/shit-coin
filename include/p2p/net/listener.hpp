#pragma once

#include "p2p/net/scopedsocket.hpp"

#include <netdb.h>
#include <thread>
#include <vector>

#define INPUT_PORT "9876"
#define BACKLOG 10
#define PACKET_SIZE 1024

class Listener {
  private:
    std::vector<std::jthread> m_workers;
    void handle_client(ScopedSocket);

  public:
    void start_listening();
};
