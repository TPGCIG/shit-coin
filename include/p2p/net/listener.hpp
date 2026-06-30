#pragma once

#include "p2p/net/scopedsocket.hpp"

#include <functional>
#include <future>
#include <netdb.h>
#include <thread>
#include <vector>

#define INPUT_PORT "9876"
#define BACKLOG 10
#define PACKET_SIZE 1024

class Listener {
  private:
    using HeaderParser = std::function<size_t(const std::byte *)>;
    using BodyParser = std::function<void(const std::byte *, size_t)>;

    std::string addr;
    std::string port;

    HeaderParser parse_header;
    BodyParser parse_body;

    std::vector<std::jthread> m_workers;
    void handle_client(ScopedSocket);

  public:
    void start_listening(std::string, std::string, std::promise<void>);

    Listener(std::string addr, std::string port, HeaderParser parse_header,
             BodyParser parse_body)
        : addr(addr), port(port), parse_header(parse_header),
          parse_body(parse_body) {};
};
