//
// Created by Ian on 9-1-2026.
//

#ifndef PERFNETPROXY_IOCONTEXT_H
#define PERFNETPROXY_IOCONTEXT_H

#include <cstdint>
#include <winsock2.h>
#include <memory>

#include "socketClient.hpp"

enum class IOOperation {
    Accept,
    Receive,
    Send,
    Unknown
};

struct IOBaseContext {
    OVERLAPPED overlapped {};
    IOOperation operation = IOOperation::Unknown;
};

struct AcceptContext {
    IOBaseContext baseContext { .operation = IOOperation::Accept };
    uint32_t bufferSize {};
    std::unique_ptr<uint8_t[]> buffer {};
    SocketClient* client {};
};

struct SendContext  {
    IOBaseContext baseContext { .operation = IOOperation::Send};
    WSABUF sendBuffer {};
    std::unique_ptr<uint8_t[]> buffer {};
};

struct ReceiveContext {
    IOBaseContext baseContext { .operation = IOOperation::Receive };
    WSABUF sendBuffer {};
    std::unique_ptr<uint8_t[]> buffer {};
};

#endif //PERFNETPROXY_IOCONTEXT_H