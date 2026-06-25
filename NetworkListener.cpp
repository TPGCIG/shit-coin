#include "NetworkListener.h"

#include <asm-generic/socket.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <string>
#include <thread>
#include <optional>
#include <unistd.h>
#include <vector>
#include <cerrno>



#define INPUT_PORT "9876"
#define BACKLOG 10
#define PACKET_SIZE 1024


void AddrInfoDeleter::operator()(struct addrinfo* res) const {
    if (res) {
        ::freeaddrinfo(res);
    }
}


ScopedSocket::ScopedSocket(ScopedSocket&& other) noexcept : m_fd(other.m_fd) {
    other.m_fd = -1;
}

ScopedSocket& ScopedSocket::operator=(ScopedSocket&& other) noexcept {
    if (this != &other) {
        if (m_fd != -1) {
            ::close(m_fd);
        }
        m_fd = other.m_fd;
        other.m_fd = -1;
    }

    return *this;
}


ScopedSocket::ScopedSocket(int fd) : m_fd(fd) {
    if (m_fd == -1) {
        std::cerr << "Warning: Invalid socket descripor.\n";
    } 
}

ScopedSocket::~ScopedSocket() {
    if (m_fd != -1) {
        ::close(m_fd);
    }
}

int ScopedSocket::get() const {
    return m_fd;
}

bool ScopedSocket::is_valid() const {
    return m_fd != -1;
}


void NetworkListener::start_listening() {

    struct addrinfo hints;
    struct addrinfo* p;

    struct addrinfo* raw_servinfo = nullptr;

    int sockfd;
    int yes = 1;
    std::optional<ScopedSocket> ssockfd;

    std::memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (::getaddrinfo("localhost", INPUT_PORT, &hints, &raw_servinfo) != 0) {
        throw std::runtime_error("GAI Failed");
    }

    SafeAddrInfo servinfo(raw_servinfo);

    for (p = servinfo.get(); p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        ssockfd.emplace(sockfd);

        if (!ssockfd->is_valid()) {
            std::cerr << "server socket" << std::endl;
            continue;
        }


        if (setsockopt(ssockfd->get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
            std::cerr << "setsockopt failed\n";
            throw std::runtime_error("Failed to bind port");
        }

        if (bind(ssockfd->get(), p->ai_addr, p->ai_addrlen) == -1) {
            std::cerr << "server: bind\n";
            continue;
        }

        break;
    }


    if (p == nullptr) {
        throw std::runtime_error("Server: failed to bind"); 
    }

    if (::listen(ssockfd->get(), BACKLOG) == -1) {
        throw std::runtime_error("Listen failed");
    }

    std::cout << "listening for connection" << std::endl;

    while(true) {
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);
        int client_fd = ::accept(ssockfd->get(), (struct sockaddr *)&their_addr, &addr_size);
    
        if (client_fd == -1) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        ScopedSocket client_socket(client_fd);

        std::cout << "Connection made with client " << client_socket.get() << "\n";

        this->m_workers.emplace_back([this, move_socket = std::move(client_socket)]() mutable {
            this->handle_client(std::move(move_socket));
        });


    }
}

int NetworkListener::handle_client(ScopedSocket socket) {
    while (true) {
        std::string message;
        message.resize(PACKET_SIZE);


        int bytes = ::recv(socket.get(), &message[0], PACKET_SIZE-1, 0);
        if (bytes == 0) {
            std::cout << "Graceful exit\n";
            break;
        }

        if (bytes == -1) {
            std::cout << "Bad exit on socket\n";
            break;
        }

        message.resize(bytes);
        std::cout << "Incoming: " << message << "\n";

    }

    return 0;

}



