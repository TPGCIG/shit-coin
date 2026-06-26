#pragma once


#include <fstream>
#include <vector>
#include <thread>
#include <string_view>
#include <unordered_set>

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

class PeerList {
private:
    std::filebuf clientFD;
    std::unordered_set<std::string_view> clients;

public:
    int add_peer(std::string_view);
};








