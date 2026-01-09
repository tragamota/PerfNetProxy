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
    uint8_t *outputBuffer {};
    uint32_t outputBufferSize {};
    SocketClient* ClientSocket {};
};

struct SendContext : IOContext {
    uint8_t *buffer {};
    uint32_t bufferSize {};
};

struct ReceiveContext : IOContext {
    uint8_t *buffer {};
    uint32_t bufferSize {};
};

#endif //PERFNETPROXY_IOCONTEXT_H