
#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <thread>
#include <vector>

void *get_in_addr(sockaddr *);

// now we play the client

class Dispatcher {
  private:
    std::vector<std::jthread> m_workers;

  public:
    int send_to_endpoint(std::string, std::string, bool, std::vector<uint8_t>);
};
