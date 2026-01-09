//
// Created by Ian on 9-1-2026.
//

#ifndef PERFNETPROXY_SOCKETCLIENT_H
#define PERFNETPROXY_SOCKETCLIENT_H

#include "InternalSocket.hpp"

class SocketClient {
    InternalClientSocket m_InternalSocket {};

public:
    SocketClient();
    ~SocketClient();

    SocketClient(const SocketClient &) = delete;
    SocketClient &operator=(const SocketClient &) = delete;

    [[nodiscard]] const InternalClientSocket *getInternalSocket() const {
        return &m_InternalSocket;
    }

    void queueSendTask() const;
    void queueSendTask(uint8_t& message) const;

    void disconnect();
};


#endif //PERFNETPROXY_SOCKETCLIENT_H