#pragma once


#include <vector>
#include <thread>

struct addrinfo;

struct AddrInfoDeleter {
    void operator()(struct addrinfo* res) const;
};

using SafeAddrInfo = std::unique_ptr<struct addrinfo, AddrInfoDeleter>;

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

class NetworkListener {
private:
    std::vector<std::jthread> m_workers;
    int handle_client(ScopedSocket);

public:
    void start_listening();


};









