#include "p2p/net/dispatcher.hpp"
#include "p2p/net/listener.hpp"
#include "p2p/net/scopedsocket.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <string>

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
// TODO: build this out with a template.

/*
int Dispatcher::send_to_peer(Peer peer, Packet pkt) {
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

        ::inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
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

    numbytes = ::send(ssockfd->get(), static_cast<void *>(&pkt), sizeof pkt, 0);

    if (numbytes == -1) {
        throw std::runtime_error("bad send");
    }
    return 0;
}
*/
int Dispatcher::request_peers(Peer peer) {
    /* Requests peers from a provided peer */

    // create the request Packet
    Packet pkt{};

    pkt.type = PacketType::RequestPeerList;

    // send it
    return this->send_to_peer(peer, pkt);
}

int Dispatcher::provide_peers(Peer peer, PeerList peer_list) {

    Packet pkt{};

    pkt.type = PacketType::PeerList;
}
