#include "p2p/net/listener.hpp"
#include "p2p/net/packets.hpp"
#include "p2p/net/scopedsocket.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <future>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void Listener::handle_client(ScopedSocket socket) {
    std::vector<std::byte> recv_buffer;
    size_t body_len{};

    while (true) {
        std::byte tmp[1024];
        int n = recv(socket.get(), tmp, sizeof tmp, 0);

        recv_buffer.insert(recv_buffer.end(), tmp, tmp + n);

        // Suck in recv information until we receive a full header, only expect
        // a non-header if explicitly told to.
        while (true) {
            if (n <= 0) {
                std::cout << "listener: connection closed with socket "
                          << socket.get() << "\n";
                return;
            }

            if (recv_buffer.empty()) {
                break;
            }

            if (body_len == 0) { // no body coming == this is a header
                if (recv_buffer.size() < sizeof(PacketHeader)) {
                    break;
                }

                // we will receive 0 if no body is coming, some value
                // representing the future body o/w
                body_len = this->parse_header(recv_buffer.data());

                recv_buffer.erase(recv_buffer.begin(),
                                  recv_buffer.begin() + sizeof(PacketHeader));

                continue;

            } // if we receive a body packet

            if (!(recv_buffer.size() >= body_len)) {
                break;
            }

            this->parse_body(recv_buffer.data(), body_len);

            recv_buffer.erase(recv_buffer.begin(),
                              recv_buffer.begin() + body_len);
            body_len = 0;
        }
    }

    std::cout << "we are closed\n";
};

void Listener::start_listening(std::string addr, std::string port,
                               std::promise<void> init_promise) {
    ::addrinfo hints;
    ::addrinfo *p;

    ::addrinfo *raw_servinfo = nullptr;

    int sockfd;
    int yes = 1;
    std::optional<ScopedSocket> ssockfd;

    std::memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (::getaddrinfo(addr.data(), port.data(), &hints, &raw_servinfo) != 0) {
        throw std::runtime_error("GAI failed");
    }

    SafeAddrInfo servinfo(raw_servinfo);

    for (p = servinfo.get(); p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        ssockfd.emplace(sockfd);

        if (!ssockfd->is_valid()) {
            std::cerr << "server socket" << std::endl;
            continue;
        }

        if (setsockopt(ssockfd->get(), SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof yes) == -1) {
            std::cerr << "setsockopt failed\n";
            throw std::runtime_error("failed to bind port");
        }

        if (bind(ssockfd->get(), p->ai_addr, p->ai_addrlen) == -1) {
            std::cerr << "server: bind\n";
            continue;
        }

        break;
    }

    if (p == nullptr) {
        throw std::runtime_error("server: failed to bind");
    }

    if (::listen(ssockfd->get(), BACKLOG) == -1) {
        throw std::runtime_error("listenn failed");
    }

    std::cout << "listening on " << ssockfd->get() << std::endl;

    init_promise.set_value();

    while (true) {
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);
        int client_fd = ::accept(ssockfd->get(), (struct sockaddr *)&their_addr,
                                 &addr_size);

        if (client_fd == -1) {
            std::cerr << "failed to accept connection\n";
            continue;
        }

        ScopedSocket client_socket(client_fd);

        std::cout << "connection made with " << client_socket.get() << "\n";

        // new thread to handle connection
        this->m_workers.emplace_back(
            [this, move_socket = std::move(client_socket)]() mutable {
                this->handle_client(std::move(move_socket));
            });
        this->m_workers.back().detach();
    }
};
