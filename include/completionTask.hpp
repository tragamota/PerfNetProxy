//
// Created by Ian on 11-1-2026.
//

#ifndef PERFNETPROXY_COMPLETIONTASK_H
#define PERFNETPROXY_COMPLETIONTASK_H

#include <variant>

#include "platform/windows/ioContext.hpp"
#include "socketClient.hpp"
#include "socketListener.hpp"

struct CompletionTask {
    std::variant<SocketClient*, SocketListener*> source {};
    size_t bytesTransferred {};
    IOContext* taskContext {};
};

#endif //PERFNETPROXY_COMPLETIONTASK_H