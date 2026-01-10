//
// Created by Ian on 9-1-2026.
//

#ifndef PERFNETPROXY_SOCKETCLIENT_H
#define PERFNETPROXY_SOCKETCLIENT_H

#include "internalSocket.hpp"
#include "internetProtocolEnums.hpp"

#include <cstdint>

class SocketClient {
    InternalClientSocket m_InternalSocket {};

public:
    explicit SocketClient(InternetProtocolVersion protocolVersion);
    ~SocketClient();

    [[nodiscard]] const InternalClientSocket *getInternalSocket() const {
        return &m_InternalSocket;
    }

    void queueSendTask(uint8_t& message) const;

    void disconnect() const;
};


#endif //PERFNETPROXY_SOCKETCLIENT_H