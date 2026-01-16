//
// Created by Ian on 11-1-2026.
//

#ifndef PERFNETPROXY_COMPLETIONTASK_H
#define PERFNETPROXY_COMPLETIONTASK_H

#include <variant>

#include "ioContext.hpp"
#include "socketClient.hpp"
#include "socketListener.hpp"

struct CompletionTask {
    std::variant<SocketClient*, SocketListener*> initSource {};
    size_t bytesTransferred {};
    IOBaseContext* taskContext {};
};

#endif //PERFNETPROXY_COMPLETIONTASK_H