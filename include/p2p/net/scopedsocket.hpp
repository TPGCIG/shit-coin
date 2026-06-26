#pragma once


#include <memory>
#include <netdb.h>

struct AddrInfoDeleter {
    void operator()(::addrinfo* res) const { ::freeaddrinfo(res); };
};

using SafeAddrInfo = std::unique_ptr<::addrinfo, AddrInfoDeleter>;


class ScopedSocket {
private:
    int m_fd = -1;

public:
    ScopedSocket() = default;

    ScopedSocket(const ScopedSocket&) = delete;

    ScopedSocket(ScopedSocket&);
    ScopedSocket(ScopedSocket&&) noexcept;
    ScopedSocket& operator=(ScopedSocket&&) noexcept;
    
    explicit ScopedSocket(int fd);
    ~ScopedSocket();

    int get() const;
    bool is_valid() const;
};

