//
// Created by Ian on 9-1-2026.
//

#ifndef PERFNETPROXY_SOCKETCLIENT_H
#define PERFNETPROXY_SOCKETCLIENT_H

#include <cstdint>
#include <span>

#include "internalSocket.hpp"
#include "internetProtocolEnums.hpp"

class SocketClient {
    InternalClientSocket m_InternalSocket {};

public:
    SocketClient() = default;
    ~SocketClient();

    void prepareSocket(InternetProtocolVersion protocolVersion);

    [[nodiscard]] const InternalClientSocket *getInternalSocket() const {
        return &m_InternalSocket;
    }

    void queueSendTask(std::span<const uint8_t> message) const;
    void queueReceiveTask(size_t receiveBufferSize = 4096) const;
    void close();
};


#endif //PERFNETPROXY_SOCKETCLIENT_H