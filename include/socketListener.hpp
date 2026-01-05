//
// Created by Ian on 1-1-2026.
//

#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <cstdint>
#include <memory>
#include <string_view>

#include "platform/windows/InternalSocket.h"

enum class InternetProtocolVersion {
    IPv4,
    IPv6
};

enum class InternetProtocolFamily {
    TCP,
    UDP
};

class SocketListener {
    std::unique_ptr<InternalSocket> m_InternalSocket;
    void cleanup() const;

public:
    explicit SocketListener(const InternetProtocolVersion& protocolVersion);
    ~SocketListener();

    SocketListener(const SocketListener&) = delete;
    SocketListener& operator=(const SocketListener&) = delete;

    SocketListener(SocketListener&&) noexcept;
    SocketListener& operator=(SocketListener&&) noexcept;

    [[nodiscard]] const InternalSocket* getSocket() const {
        return m_InternalSocket.get();
    }

    void bind(const std::string_view& bind_address, uint16_t port) const;
    void listen() const;
    void postAccept() const;
    void close() const;
};

#endif // SOCKET_LISTENER_H