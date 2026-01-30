//
// Created by Ian on 12-1-2026.
//

#include "proxyApplication.hpp"

#include <iostream>
#include <ostream>

void ProxyApplication::initializeAcceptConnections() {

}

void ProxyApplication::HandleAcceptCompletion(const CompletionTask &completionContext) {
    const auto acceptContext = std::unique_ptr<AcceptContext>(
        reinterpret_cast<AcceptContext *>(completionContext.taskContext));
    const auto sourceListener = std::get<SocketListener *>(completionContext.initSource);
    const auto client = acceptContext->client;

    m_CompletionQueue.AddSocketClientReference(*client);

    if (setsockopt(
            client->getInternalSocket()->socket,
            SOL_SOCKET,
            SO_UPDATE_ACCEPT_CONTEXT,
            reinterpret_cast<const char *>(&sourceListener->getInternalSocket()->socket),
            sizeof(sourceListener->getInternalSocket()->socket)) == SOCKET_ERROR) {
        std::cout << "Failed to set accept context" << WSAGetLastError() << std::endl;
    }

    int optval = 1;

    if (setsockopt(client->getInternalSocket()->socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&optval),
                   sizeof(int)) == SOCKET_ERROR) {
        std::cout << "Failed to set TCP_NODELAY" << WSAGetLastError() << std::endl;
    }

    client->queueReceiveTask();

    sourceListener->acceptClient();

    // m_IncomingClients.emplace_back(*sourceListener->acceptClient());
}

void ProxyApplication::HandleSendCompletion(CompletionTask &completionContext) {
    const auto sendContext = std::unique_ptr<SendContext>(
        reinterpret_cast<SendContext *>(completionContext.taskContext));
    const auto client = std::get<SocketClient *>(completionContext.initSource);

    if (completionContext.bytesTransferred == 0) {
        client->close();
    }
}

void ProxyApplication::HandleReceiveCompletion(CompletionTask &completionContext) {
    const auto client = std::get<SocketClient *>(completionContext.initSource);
    const auto receiveContext = std::unique_ptr<ReceiveContext>(
        reinterpret_cast<ReceiveContext *>(completionContext.taskContext));

    if (completionContext.bytesTransferred == 0) {
        client->close();
    }

    std::cout << receiveContext->buffer.get() << std::endl;

    client->queueReceiveTask();
}

ProxyApplication::ProxyApplication() {
    auto ipv4Listener = SocketListener{InternetProtocolVersion::IPv4};
    auto ipv6Listener = SocketListener(InternetProtocolVersion::IPv6);

    ipv4Listener.bind("0.0.0.0", 8080);
    ipv6Listener.bind("::", 8080);

    ipv4Listener.listen();
    ipv6Listener.listen();

    m_Listeners.emplace_back(std::move(ipv4Listener));
    m_Listeners.emplace_back(std::move(ipv6Listener));

    for (const auto &listener: m_Listeners) {
        m_CompletionQueue.AddSocketListenerReference(listener);

        for (int i = 0; i < 512; i++) {
            listener.acceptClient();
        }
    }
}

void ProxyApplication::run() {
    m_IsRunning = true;

    while (m_IsRunning) {
        switch (auto completedTask = m_CompletionQueue.GetCompletionTask(); completedTask.taskContext->operation) {
            case IOOperation::Accept:
                HandleAcceptCompletion(completedTask);
                break;
            case IOOperation::Receive:
                HandleReceiveCompletion(completedTask);
                break;
            case IOOperation::Send:
                HandleSendCompletion(completedTask);
                break;
            case IOOperation::Unknown:
                std::cout << "Unknown operation" << std::endl;
                break;
        }
    }

    stop();
}
