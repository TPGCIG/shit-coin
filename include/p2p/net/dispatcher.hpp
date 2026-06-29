
#pragma once

#include "p2p/net/packets.hpp"
#include "p2p/net/peers.hpp"
#include "p2p/net/scopedsocket.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <thread>
#include <vector>

void *get_in_addr(sockaddr *);

// now we play the client

class Dispatcher {
  private:
    std::vector<std::jthread> m_workers;

    template <typename T>
    int send_to_peer(
        Peer peer, std::vector<uint8_t> header,
        std::optional<std::vector<uint8_t>> packet_body = std::nullopt) {

        int sockfd, numbytes;
        std::string buf;
        ::addrinfo hints;
        ::addrinfo *p;
        ::addrinfo *raw_servinfo = nullptr;
        std::optional<ScopedSocket> ssockfd;

        int rv;
        char s[INET6_ADDRSTRLEN];

        std::memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        rv = ::getaddrinfo(peer.address.data(), peer.port.data(), &hints,
                           &raw_servinfo);

        if (rv != 0) {
            std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
            return 1;
        }

        SafeAddrInfo servinfo(raw_servinfo);

        // loop through all the results and connect to the first we can
        for (p = servinfo.get(); p != nullptr; p = p->ai_next) {
            sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            ssockfd.emplace(sockfd);
            if (!ssockfd->is_valid()) {
                std::cerr << "socket\n";
                continue;
            }

            ::inet_ntop(p->ai_family,
                        get_in_addr((struct sockaddr *)p->ai_addr), s,
                        sizeof s);
            std::cout << "client: attempting connection to " << s << "\n";

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                perror("client: connect");
                continue;
            }

            break;
        }

        if (p == nullptr) {
            fprintf(stderr, "client: failed to connect\n");
            return 2;
        }

        ::inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
                    sizeof s);

        std::cout << "client: connected to " << s << "\n";

        numbytes = ::send(ssockfd->get(), header.data(), header.size(), 0);

        std::cout << "client: sent " << numbytes << " bytes over the wire!\n";

        if (numbytes == -1) {
            throw std::runtime_error("bad send");
        }

        if (!packet_body.has_value()) {
            return 0;
        }

        numbytes =
            ::send(ssockfd->get(), packet_body->data(), packet_body->size(), 0);
        if (numbytes == -1) {
            throw std::runtime_error("bad send");
        }
        std::cout << "Sent another " << numbytes << " over the cable\n";
        getchar();
        return 0;
    }

  public:
    int request_peers(Peer);
    int send_peers(Peer, PeerList *);
};
