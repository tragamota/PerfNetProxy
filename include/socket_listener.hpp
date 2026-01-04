//
// Created by Ian on 1-1-2026.
//

#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <cstdint>
#include <memory>
#include <string_view>

enum class InternetProtocolVersion {
    IPv4,
    IPv6
};

enum class InternetProtocolFamily {
    TCP,
    UDP
};

struct internalSocket;

class SocketListener {
    std::unique_ptr<internalSocket> m_InternalSocket;
    void cleanup() const;

public:
    explicit SocketListener(const InternetProtocolVersion& protocolVersion);
    ~SocketListener();

    SocketListener(const SocketListener&) = delete;
    SocketListener& operator=(const SocketListener&) = delete;

    SocketListener(SocketListener&&) noexcept;
    SocketListener& operator=(SocketListener&&) noexcept;

    void bind(const std::string_view& bind_address, uint16_t port) const;
    void listen() const;
    void postAccept() const;
    void close();
};

#endif // SOCKET_LISTENER_H