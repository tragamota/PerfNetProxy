//
// Created by Ian on 9-1-2026.
//

#ifndef PERFNETPROXY_IOCONTEXT_H
#define PERFNETPROXY_IOCONTEXT_H

#include <cstdint>
#include <winsock2.h>

#include "socketClient.hpp"

enum class IOOperation {
    Accept,
    Receive,
    Send,
    Unknown
};

struct IOContext {
    OVERLAPPED overlapped {};
    IOOperation operation = IOOperation::Unknown;
};

struct AcceptContext : IOContext {
    uint32_t outputBufferSize {};
    uint8_t *outputBuffer {};
    SocketClient* ClientSocket {};
};

struct SendContext : IOContext {
    WSABUF sendBuffer {};
    uint8_t *buffer {};
};

struct ReceiveContext : IOContext {
    WSABUF sendBuffer {};
    uint8_t *buffer {};
};

#endif //PERFNETPROXY_IOCONTEXT_H