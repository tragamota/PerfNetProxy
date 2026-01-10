//
// Created by Ian on 1-1-2026.
//

#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <cstdint>
#include <string>

#include "internalSocket.hpp"
#include "internetProtocolEnums.hpp"
#include "socketClient.hpp"

class SocketListener {
    InternalListeningSocket m_InternalSocket{};
    InternetProtocolVersion m_ProtocolVersion;

    void cleanup();

public:
    explicit SocketListener(const InternetProtocolVersion &protocolVersion);

    ~SocketListener() {
        cleanup();
    }

    SocketListener(const SocketListener &) = delete;
    SocketListener &operator=(const SocketListener &) = delete;

    SocketListener(SocketListener &&other) noexcept : m_InternalSocket(other.m_InternalSocket),
                                                      m_ProtocolVersion(other.m_ProtocolVersion) {
    }

    SocketListener &operator=(SocketListener &&other) noexcept {
        if (this != &other)
            m_InternalSocket = other.m_InternalSocket;
        return *this;
    }

    [[nodiscard]] const InternalListeningSocket *getInternalSocket() const {
        return &m_InternalSocket;
    }

    void bind(const std::string &bind_address, uint16_t port);
    void listen() const;

    SocketClient* acceptClient() const;
    void close();
};

#endif // SOCKET_LISTENER_H
