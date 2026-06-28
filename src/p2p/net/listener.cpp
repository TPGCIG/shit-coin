#include "p2p/net/listener.hpp"
#include "p2p/net/dispatcher.hpp"
#include "p2p/net/packets.hpp"
#include "p2p/net/scopedsocket.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void Listener::handle_client(ScopedSocket socket) {
    std::vector<std::byte> recv_buffer;
    PacketHeader pkt_h{};

    bool header_incoming = true;
    bool header_received = false;

    std::byte tmp[1024];
    int n = recv(socket.get(), tmp, sizeof tmp, 0);

    recv_buffer.insert(recv_buffer.end(), tmp, tmp + n);

    // Suck in recv information until we receive a full header, only expect a
    // non-header if explicitly told to.
    while (true) {

        if (n <= 0) {
            std::cout << "listener: connection closed with socket "
                      << socket.get() << "\n";
            return;
        }

        if (header_incoming) {
            if (recv_buffer.size() >= sizeof(PacketHeader)) {
                pkt_h = deserialise_header_pkt(recv_buffer.data());
                header_received = true;
                std::cout << "header received with: \n"
                          << "    | type = "
                          << static_cast<int>(pkt_h.packet_type) << "\n"
                          << "    | len  = " << pkt_h.length << "\n";
            }

            break;

        } else { // if we receive a body packet
            if (!(recv_buffer.size() >= pkt_h.length)) {
                break;
            }

            switch (static_cast<PacketType>(pkt_h.packet_type)) {
            case PacketType::PeerList: {
                PeerListPacket plp =
                    deserialise_peer_list_pkt(recv_buffer.data());
                std::cout << "Received peerlistpacket with count: "
                          << plp.peer_count << "\n";
                break;
            }
            default:
                std::cerr << "listener: catastrophic error - packet type: "
                          << static_cast<int>(pkt_h.packet_type) << "\n";
                return;
            }

            header_incoming = true;
        }
    }

    if (header_received) {
        header_incoming =
            is_bodyless(static_cast<PacketType>(pkt_h.packet_type));
        header_received = false;
    }
};

void Listener::start_listening() {
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

    if (::getaddrinfo("localhost", INPUT_PORT, &hints, &raw_servinfo) != 0) {
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

    std::cout << "listening for connection" << std::endl;

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
