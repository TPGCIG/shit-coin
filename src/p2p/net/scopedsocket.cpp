#include "p2p/net/scopedsocket.hpp"

#include <iostream>

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
        std::cerr << "invalid socket descripor, dont use\n";
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
