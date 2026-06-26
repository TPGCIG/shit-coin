#include "p2p/net/dispatcher.hpp"
#include "p2p/net/scopedsocket.hpp"

#include <netinet/in.h>
#include <optional>
#include <string>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Dispatcher::connect_to_peer(Peer peer) {
    int sockfd, numbytes;  
    std::string buf;
    ::addrinfo hints;
    ::addrinfo *p;
    ::addrinfo* raw_servinfo = nullptr;
    std::optional<ScopedSocket> ssockfd;


    int rv;
    std::string s;

    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    rv = ::getaddrinfo(peer.address.data(), peer.port.data(), &hints, &raw_servinfo);

    if (rv != 0) {
        std::cerr <<  "getaddrinfo: " << gai_strerror(rv) << "\n";
        return 1;
    }

    SafeAddrInfo servinfo(raw_servinfo);

    // loop through all the results and connect to the first we can
    for(p = servinfo.get(); p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        ssockfd.emplace(sockfd);
        if (!ssockfd->is_valid()) {
            std::cerr << "socket\n";
            continue;
        }


        ::inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s.data(), sizeof s);
        std::cout << "client: attempting connection to " <<  s.data() << "\n";

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    ::inet_ntop(p->ai_family,
            get_in_addr((struct sockaddr *)p->ai_addr),
            s.data(), sizeof s);
    std::cout << "client: connected to " <<  s.data() << "\n";

    numbytes = ::recv(sockfd, buf.data(), 1024, 0);

    if (numbytes == -1) {
        throw std::runtime_error("bad recv");
    }

    std::cout << "client: received " << buf.data();

    return 0;



}

