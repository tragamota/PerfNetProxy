//
// Created by Ian on 12-1-2026.
//

#ifndef PERFNETPROXY_PROXYAPPLICATION_H
#define PERFNETPROXY_PROXYAPPLICATION_H

#include <vector>

#include "completionQueue.hpp"
#include "platformInitializer.hpp"
#include "socketClient.hpp"

class ProxyApplication {
    PlatformInitializer m_PlatformInitializer {};
    CompletionQueue m_CompletionQueue {};

    std::vector<SocketListener> m_listeners {};
    std::vector<SocketClient> m_incomingClients {};
    std::vector<SocketClient> m_clients {};
public:
    ProxyApplication();
    ~ProxyApplication();


};


#endif //PERFNETPROXY_PROXYAPPLICATION_H