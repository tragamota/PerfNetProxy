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

    std::vector<SocketListener> m_Listeners {};
    std::vector<SocketClient> m_IncomingClients {};
    std::vector<SocketClient> m_ActiveClients {};

    void initializeAcceptConnections();

    void HandleAcceptCompletion(const CompletionTask&);
    void HandleSendCompletion(CompletionTask&);
    void HandleReceiveCompletion(CompletionTask&);

    std::atomic<bool> m_IsRunning = false;
public:
    ProxyApplication();
    ~ProxyApplication() = default;

    void run();
    void stop() {
        m_IsRunning = false;
    }
};


#endif //PERFNETPROXY_PROXYAPPLICATION_H